#include "network.h"
#include "net/server.h"

NAMESPACE_BEGIN(gb)
using ListenMap = typename std::unordered_map<uint64_t,net_listen_fun>;
using RpcInterfaceMap = typename  std::map<uint64_t, rpc_listen_fun>;
using RpcCallerMap = typename std::map<int,std::map<uint64_t, RpcCall>>;
//using RpcCallerMap = typename std::map<uint64_t, RpcCall>;
//using RpcCallerMap = typename std::map<uint32_t,std::map<uint64_t, RpcCall>>;
static thread_local ListenMap gs_ListenFunctionMap;
static thread_local RpcInterfaceMap gs_RpcInterfaceMap;
static thread_local RpcCallerMap gs_RpcCallerMap;
static thread_local int32_t gs_sequence_tail = 0;

static std::unique_ptr<Server> gs_Server;

uint64_t GetSequence()
{
	std::thread::id id = std::this_thread::get_id();
	uint32_t thread_id = *((uint32_t*)&id);
	return (((uint64_t)thread_id) << 32) + (++gs_sequence_tail);
}

void UnListen(int type, int id, std::string signal, int level)
{
	uint64_t key = (((uint64_t)type) << 32) + id;
	auto p_FunsIt = gs_ListenFunctionMap.find(key);
	if (gs_ListenFunctionMap.end() == p_FunsIt)
	{
		LOG_ERROR("message don't listen type:{} id:{}", type, id);
		return;
	}
	{
        gs_ListenFunctionMap.erase(p_FunsIt);
	}
}

void Send(Session* session, int type, int id, google::protobuf::Message& msg)
{
	Meta meta;
	meta.set_id(id);
	meta.set_type(type);
	session->Send(&meta, &msg);
}
void Send(std::shared_ptr<Session> session, int type, int id, google::protobuf::Message& msg)
{
	Meta meta;
	meta.set_id(id);
	meta.set_type(type);
	session->Send(&meta, &msg);

}


void ListenOption(int type, int id, net_listen_fun f, std::string protoName)
{
	uint64_t key = (((uint64_t)type) << 32) + id;
	auto p_FunsIt = gs_ListenFunctionMap[key] = f;
}

void _Call(Meta & meta, RpcCall call)
{
    uint32_t thread_id  = uint32_t(meta.sequence() >> 32);
    auto&    thread_map = gs_RpcCallerMap[thread_id];
    if (!thread_map.insert({meta.sequence(), call}).second)
    {
        LOG_ERROR("insert gs_RpcCallerMap fail seq:{} method{}", meta.sequence(), meta.method());
    }
    //开启计时器

    //调用回调
    call.Call(meta);
}

void _Call(Meta& meta, RpcCall call,std::vector<uint8_t>& data)
{
    uint32_t thread_id  = uint32_t(meta.sequence() >> 32);
    auto&    thread_map = gs_RpcCallerMap[thread_id];
    if (!thread_map.insert({meta.sequence(), call}).second)
    {
        LOG_ERROR("insert gs_RpcCallerMap fail seq:{} method{}", meta.sequence(), meta.method());
    }
    //开启计时器

    //调用回调
    call.Call(meta, data);
}

void _Call(Meta & meta, RpcCall call, std::vector<uint8_t>&& data)
{
    uint32_t thread_id  = uint32_t(meta.sequence() >> 32);
    auto&    thread_map = gs_RpcCallerMap[thread_id];
    if (!thread_map.insert({meta.sequence(), call}).second)
    {
        LOG_ERROR("insert gs_RpcCallerMap fail seq:{} method{}", meta.sequence(), meta.method());
    }
    //开启计时器

    //调用回调
    call.Call(meta, data);

}
void _Call(RpcCall call, std::string method, sol::variadic_args& args)
{
    Meta meta;
    uint64_t    method_key = MD5::MD5Hash64(method.c_str());
    meta.set_method(method_key);
    uint64_t seq_id = GetSequence();
    meta.set_sequence(seq_id);
    meta.set_mode(MsgMode::Request);
    call.SetId(seq_id);
    if (args.size() > 0)
    {
        std::vector<uint8_t> data = gb::msgpack::pack(args);
        _Call(meta, call, std::move(data));
    }
    else
    {
        _Call(meta, call);
    }
}


void RegisterOption(std::string method, rpc_listen_fun fn)
{
	uint64_t key = MD5::MD5Hash64(method.c_str());
	gs_RpcInterfaceMap.insert({key,fn});
}

void UnRegister(std::string method)
{
	uint64_t key = MD5::MD5Hash64(method.c_str());
	gs_RpcInterfaceMap.erase(key);
}


void Dispatch(const SessionPtr& session, const ReadBufferPtr& buffer, Meta& meta, int meta_size, int64_t data_size)
{
    gb::IoServicePoolPtr  io_service_pool = gs_Server->GetIoServicePool();
    if (!io_service_pool)
        return;
	switch (meta.mode())
	{
    case MsgMode::Msg:
	{
		auto worker = io_service_pool->GetWorker(session->GetIoServicePoolIndex());
		if (!worker.has_value())
			return;
		worker.value()->Post([session = session, buffer = buffer ,meta = std::move(meta), meta_size,data_size]() mutable
			{
				uint64_t key = (((uint64_t)meta.type()) << 32) + (int)meta.id();
				auto fun = gs_ListenFunctionMap.find(key);
				if (fun != gs_ListenFunctionMap.end())
					fun->second(session, buffer,meta,meta_size,data_size);
			});
		break;
	}

	case MsgMode::Request:
    {
        uint64_t seq       = meta.sequence();
        uint32_t thread_id = uint32_t(seq >> 32);
        auto     worker    = io_service_pool->GetWorker(session->GetIoServicePoolIndex());
        if (worker.has_value())
        {
            uint64_t key  = meta.method();
            auto     func = gs_RpcInterfaceMap.find(key);
            if (func == gs_RpcInterfaceMap.end())
                return;

            auto&                f = func->second;
            std::vector<uint8_t> vec;
            f(session, buffer, meta, meta_size, data_size);
        }
        break;
    }
	case MsgMode::Response:
	{
		uint64_t seq = meta.sequence();
		uint32_t thread_id = uint32_t(seq >> 32);

		auto worker = io_service_pool->GetWorker(session->GetIoServicePoolIndex());
		if (worker.has_value())
		{
			std::thread::id id = std::this_thread::get_id();
			uint32_t this_thread_id = *((uint32_t*)&id);
			//if (this_thread_id != thread_id)
			//	return;
			auto& thread_map = gs_RpcCallerMap[thread_id];
			auto it = thread_map.find(seq);
			if (it == thread_map.end())
				return;

			if(it->second.HasCallBack())
				it->second.Done(session,buffer,meta,meta_size,data_size);
			thread_map.erase(it);
		}
		break;
	}
	default:
		break;
	}

}



void net_init()
{
	auto [ip, port] = AppTypeMgr::Instance()->GetServerIpPort();
	std::string uir = ip + ":" + port;

	gb::ServerOptions options;
    options.keep_alive_time = -1;
    options.io_service_pool_size = 1;
    gs_Server               = std::make_unique<gb::Server>(options);
    gs_Server->SetAcceptCallBack([](const SessionPtr& session) {
        session->set_return_io_service_pool_fun([]() -> gb::IoServicePoolPtr {
            return gs_Server->GetIoServicePool();
        });
        LOG_INFO("Accept:{}", session->socket().local_endpoint().address().to_string());
    });
    gs_Server->SetCloseCallBack([](const SessionPtr& session) {
        LOG_INFO("Close:{}", session->socket().local_endpoint().address().to_string());
    });
    gs_Server->SetReceivedCallBack([](const SessionPtr& session, const ReadBufferPtr& buffer,int meta_size,int64_t data_size) {
        Meta meta;
		if (meta.ParseFromBoundedZeroCopyStream(buffer.get(), meta_size))
		{
            Dispatch(session, buffer, meta, meta_size, data_size);
		}
		
    });
	
	if (!gs_Server->Start(uir))
	{
        LOG_ERROR("server start fail");
	}
}

gb::IoServicePoolPtr net_get_io_service_pool()
{
    return gs_Server->GetIoServicePool();
}

//gb::IoServicePoolPtr net_GetIoServicePool()
//{
//}


NAMESPACE_END

