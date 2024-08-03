#include "message_stream.h"
#include "../common_component/endpoint_help.h"
#include <algorithm>
namespace gb
{





 MessageStream::MessageStream(NET_TYPE net_type, IoService& io_service, const Endpoint& endpoint) 
	: SSLByteStream(net_type,io_service,endpoint)
	, _pending_message_count(0)
	, _pending_data_size(0)
	, _pending_buffer_size(0)
	, _swapped_message_count(0)
	, _swapped_data_size(0)
	, _swapped_buffer_size(0)
	, _max_pending_buffer_size(0)
	, _read_quota_token(1)
	, _write_quota_token(1)
	, _total_sent_count(0)
	, _total_sent_size(0)
	, _total_received_count(0)
	, _total_received_size(0)
	, _sent_size(0)
	, _sending_data(nullptr)
	, _sending_size(0)
	, _received_message_size(0)
	, _received_header_size(0)
	, _receiving_header_identified(false)
	, _tran_buf(nullptr)
	, _receiving_data(nullptr)
	, _receiving_size(0)
	, _send_token(TOKEN_FREE)
	, _receive_token(TOKEN_FREE)
{
}

 MessageStream::~MessageStream()
{
	 if (_tran_buf != nullptr)
	 {
         TranBufPool::free(_tran_buf);
         _tran_buf = nullptr;
	 }
 }

void MessageStream::async_send_message(const ReadBufferPtr& message)
{
	if (is_closed())
	{
        on_send_failed("status is close", message);
        return;
	}
	
	put_into_pending_queue(message);
    try_start_send();
}

void MessageStream::set_flow_controller(const FlowControllerPtr& flow_controller)
{
    _flow_controller = flow_controller;
}

void MessageStream::set_max_pending_buffer_size(int64_t max_pending_buffer_size)
{
    _max_pending_buffer_size = max_pending_buffer_size;
}

int64_t MessageStream::max_pending_buffer_size() const
{
    return _max_pending_buffer_size;
}

int64_t MessageStream::pending_message_count() const
{
    return _pending_message_count + _swapped_message_count;
}

int64_t MessageStream::pending_data_size() const
{
    return _pending_data_size + _swapped_data_size;
}

int64_t MessageStream::pending_buffer_size() const
{
    return _pending_buffer_size + _swapped_buffer_size;
}

bool MessageStream::trigger_receive()
{
    _read_quota_token = 1;
    return try_start_receive();
}

bool MessageStream::trigger_send()
{
    _write_quota_token = 1;
    return try_start_send();
}

int MessageStream::read_quota_token() const
{
    return _read_quota_token;
}

int MessageStream::write_quota_token() const
{
    return _write_quota_token;
}

//bool MessageStream::on_sending(const ReadBufferPtr& message)
//{
//}
//
//void MessageStream::on_sent(const ReadBufferPtr& message)
//{
//}
//
//void MessageStream::on_send_failed(std::string_view peason, const ReadBufferPtr& message)
//{
//}
//
//void MessageStream::on_received(const ReadBufferPtr& message, int meta_size, int64_t data_size)
//{
//}

bool MessageStream::on_connected()
{
	//重置接收数据环境
    reset_receiving_env();
	if (!reset_tran_buf())
	{
        close("out of memory");
        return false;
	}
    return true;
}

void MessageStream::on_read_some(const Error_code& error, size_t bytes_transferred)
{
	if (!is_connected())
        return;

	if (error)
	{
		if (error != Asio::error::eof)
            NETWORK_LOG("on_read_some(): {}: read error: {}", EndpointToString(_remote_endpoint), error.message());
        
		close(error.message());
		return;
	}

	_last_rw_ticks = _ticks;
    ++_total_received_count;
    _total_received_size += bytes_transferred;

	std::deque<ReceivedItem> received_message;
	if (!split_and_process_message(_receiving_data, static_cast<int>(bytes_transferred), &received_message))
	{
        close("broken stream");
        return;
	}

	_receiving_data += bytes_transferred;
    _receiving_size -= bytes_transferred;

	if (_receiving_size == 0 && !reset_tran_buf())
	{
        close("out of memory");
        return;
	}

	atomic_comp_swap(&_receive_token, TOKEN_FREE, TOKEN_LOCK);

	//继续去读
	try_start_receive();

	//交给上层处理
	while (!is_closed() && !received_message.empty())
	{
        const ReceivedItem& item = received_message.front();
        on_received(item.message, item.meta_size, item.data_size);
        received_message.pop_front();
	}
}


void MessageStream::on_write_some(const Error_code& error, size_t bytes_transferred)
{

    if (!is_connected())
        return;

	if (error)
	{
        NETWORK_LOG("on_write_some(): {}: write error: {}", EndpointToString(_remote_endpoint), error.message());
        on_send_failed(error.message(), _sending_message);
        clear_sending_env();
        close(error.message());
        return;
	}
	
	_last_rw_ticks = _ticks;
    _sent_size += bytes_transferred;
    ++_total_sent_count;
    _total_sent_size += bytes_transferred;
    if (static_cast<int>(bytes_transferred) == _sending_size)
	{
		if (_sending_message->Next(reinterpret_cast<const void**>(&_sending_data), &_sending_size))
		{
            async_write_some(_sending_data, _sending_size);
		}
		else
		{
			//检查消息是否全部发送
            CHECK_EQ(_sent_size, _sending_message->ByteCount());
			
			on_sent(_sending_message);
			
			atomic_comp_swap(&_send_token, TOKEN_FREE, TOKEN_LOCK);

			try_start_send();

			//由于发送缓冲区满了  读取会被挂起  所以处理完读取一定要唤醒读操作
			if (_net_type == NET_TYPE::NT_SERVER)
                try_start_receive();
		}
	}
	else
	{
		//检查是否只发了一部分数据
		CHECK_LT(static_cast<int>(bytes_transferred), _sending_size);
		_sending_data += bytes_transferred;
        _sending_size -= bytes_transferred;
        async_write_some(_sending_data, _sending_size);
	}

}

void MessageStream::put_into_pending_queue(const ReadBufferPtr& message)
{
    std::lock_guard<std::mutex> _lock(_mutex);
    _pending_calls.push_back(PendingItem(message));
    ++_pending_message_count;
    _pending_data_size += message->TotalCount();
    _pending_buffer_size += message->TotalBlockSize();
}

void MessageStream::insert_into_pending_queue(const ReadBufferPtr& message)
{
    _swapped_calls.push_front(PendingItem(message));
    ++_swapped_message_count;
    _swapped_data_size += message->TotalCount();
    _swapped_buffer_size += message->TotalBlockSize();
}

bool MessageStream::get_from_pending_queue(ReadBufferPtr* message)
{
	if (_swapped_calls.empty() && _pending_message_count > 0)
	{
        std::lock_guard<std::mutex> _lock(_mutex);
		if (!_pending_calls.empty())
		{
			_swapped_calls.swap(_pending_calls);
			_swapped_message_count = _pending_message_count;
			_swapped_data_size = _pending_data_size;
			_swapped_buffer_size = _pending_buffer_size;
			_pending_message_count = 0;
			_pending_data_size = 0;
			_pending_buffer_size = 0;
		}
	}

	if (!_swapped_calls.empty())
	{
        *message = _swapped_calls.front().message;
        _swapped_calls.pop_front();
        --_swapped_message_count;
        _swapped_data_size -= (*message)->TotalCount();
        _swapped_buffer_size -= (*message)->TotalBlockSize();
        return true;
	}

	CHECK_EQ(0, _swapped_message_count);
    CHECK_EQ(0, _swapped_data_size);
    CHECK_EQ(0, _swapped_buffer_size);
    return false;
}

bool MessageStream::try_start_receive()
{
    if (_receive_token == TOKEN_LOCK)
        return false;

	//上一次已经达到流量上限了
	if (_read_quota_token <= 0)
        return false;  
	
	if (_net_type == NET_TYPE::NT_SERVER && pending_buffer_size() > max_pending_buffer_size())
        return false;		//先去把该发送的发了
	
	bool started = false;
	if (is_connected() && atomic_comp_swap(&_receive_token, TOKEN_LOCK, TOKEN_FREE) == TOKEN_FREE)  //之前是free 现在给他lock   即做判断也做上锁操作
	{
        CHECK(_receiving_data != nullptr);
        CHECK(_receiving_size > 0);
		if ((_read_quota_token = _flow_controller->acquire_read_quota(_receiving_size)) <= 0)	//判断当前是否还有流量
		{
			//没流量就不操作了 解锁
            atomic_comp_swap(&_receive_token, TOKEN_FREE, TOKEN_LOCK);
		}
		else
		{
            async_read_some(_receiving_data, _receiving_size);
            started = true;
		}
	}
    return started;

}

bool MessageStream::try_start_send()
{
    if (_send_token == TOKEN_LOCK)
        return false;

	if (_write_quota_token <= 0)
        return false;

	bool started = false;
	while (is_connected() && atomic_comp_swap(&_send_token, TOKEN_LOCK, TOKEN_FREE) == TOKEN_FREE)
	{
		if (get_from_pending_queue(&_sending_message))
		{
			if (!on_sending(_sending_message))
			{
                on_send_failed("on_sending() return false", _sending_message);
                clear_sending_env();
                atomic_comp_swap(&_send_token, TOKEN_FREE, TOKEN_LOCK);
                break;
			}
			else if ((_write_quota_token = _flow_controller->acquire_write_quota(_sending_message->TotalCount())) <= 0)
			{
                insert_into_pending_queue(_sending_message);
                clear_sending_env();
                atomic_comp_swap(&_send_token, TOKEN_FREE, TOKEN_LOCK);
                break;
			}
			else
			{
                _sent_size = 0;
				bool ret = _sending_message->Next(reinterpret_cast<const void**>(&_sending_data), &_sending_size);
                CHECK(ret);
                async_write_some(_sending_data, _sending_size);
                started = true;
                break;

			}

		}
		else
		{

			atomic_comp_swap(&_send_token, TOKEN_FREE, TOKEN_LOCK);
			//防止其他线程 在这个函数中插入数据
			//因为这个线程占用着token 导致try_start_send失败
			//所以要检查下 消息数量是否为0  不为0说明 有其他线程插入数据了  需要继续取数据发送数据
            if (_pending_message_count == 0)
                break;
		}
	}

	return started;
}

bool MessageStream::split_and_process_message(char* data, int size, std::deque<ReceivedItem>* received_messages)
{
	while (size > 0)
	{
		if (!_receiving_header_identified)
		{
            int consumed = 0;
            int identify_result = identify_message_header(data, size, &consumed);
			if (identify_result > 0)
			{
                _receiving_header_identified = true;
				if (consumed == size && _receiving_header.message_size > 0)
				{
					//消息是空的 只有头
                    return true;
				}
				else
				{
					//偏移出消息头的大小 只存放meta和数据
                    data += consumed;//输入指针往后便宜已经读取的大小
                    size -= consumed;//更新剩余数据大小  
				}
			}
			else if (identify_result == 0)
			{
                return true;
			}
			else 
			{
                return false;	
			}
		}

		int64_t message_size = _receiving_header.message_size;
		if (_received_message_size + size < message_size)
		{
            _receiving_message->Append(BufferHandle(_tran_buf, size, data - _tran_buf));
            _received_message_size += size;
            return true;
		}

		int64_t consume_size = message_size - _received_message_size;
		if (consume_size > 0)
		{
            _receiving_message->Append(BufferHandle(_tran_buf, consume_size, data - _tran_buf));
		}
        received_messages->push_back(ReceivedItem(_receiving_message, _receiving_header.meta_size, _receiving_header.data_size));
        /*以上一条消息解析出来了 接着开始下一条*/
		reset_receiving_env();
        data += consume_size;
        size -= consume_size;

	}
    return true;
}


/**
 * return 1 报文头存放在 _receiving_header中
 * return 0 由于数据不完整没有标识头 将数据复制到 _receiving_header中
 * return -1 消息流中断
 */
int MessageStream::identify_message_header(char* data, int size, int* consumed)
{
    int header_size = static_cast<int>(sizeof(_receiving_header));
    int copy_size   = std::min(size, header_size - _received_header_size);  //可能有一些头信息之前读了一半
    memcpy(reinterpret_cast<char*>(&_receiving_header) + _received_header_size, data, copy_size);

	_received_header_size += copy_size;
    *consumed = copy_size;

	if (_received_header_size < header_size)	//还没完成解析消息头
    {
		return 0;
    }
	else
	{
		if (!_receiving_header.CheckMagicString())
		{
            NETWORK_LOG("identify_message_header(): {}: check magic string failed: {}", EndpointToString(_remote_endpoint), _receiving_header.magic_str_value);
            return -1;
		}
		if (_receiving_header.meta_size + _receiving_header.data_size != _receiving_header.message_size)
		{
            NETWORK_LOG("identify_message_header(): {}: check size in header failed: meta_size={}, data_size={}, message_size={}", EndpointToString(_remote_endpoint), _receiving_header.meta_size,
                        _receiving_header.data_size, _receiving_header.message_size);
		}
        return 1;
	}
}

void MessageStream::clear_sending_env()
{
	_sending_message.reset();
	_sent_size = 0;
    _sending_data = nullptr;
	_sending_size = 0;
}

void MessageStream::reset_receiving_env()
{
	_receiving_message.reset(new ReadBuffer());
	_received_message_size = 0;
	_received_header_size = 0;
	_receiving_header_identified = false;
}

bool MessageStream::reset_tran_buf()
{
	if (_tran_buf != nullptr)
	{
        TranBufPool::free(_tran_buf);		//释放旧的buf  减少一次引用
        _tran_buf = nullptr;
	}
	
	_tran_buf = reinterpret_cast<char*>(TranBufPool::malloc(_read_buffer_base_block_factor));
	if (_tran_buf == nullptr)
	{
        NETWORK_LOG("reset_tran_buf(): {}: malloc buffer failed: out of memory", EndpointToString(_remote_endpoint));
        return false;
	}
	
	_receiving_data = reinterpret_cast<char*>(_tran_buf);
    _receiving_size = TranBufPool::capacity(_tran_buf);
    return true;

}



}