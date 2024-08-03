#include "rpc_call.h"
#include "../common_component/timer_help.h"

namespace gb
{


RpcCall::RpcCall()
{
}

RpcCall::~RpcCall()
{
}

void RpcCall::SetTimeout(std::function<void()> timeout_fun, int64_t timeout)
{
    timeout_      = std::chrono::milliseconds(timeout);
    timeout_func_ = timeout_fun;
};

void RpcCall::SetTimeout(int64_t timeout)
{
    timeout_ = std::chrono::milliseconds(timeout);
    Asio::steady_timer timer(session_->ioservice(),timeout_);
}

void RpcCall::Call(Meta& meta)
{
    if (!session_)
        return;
        session_->Send(&meta);
}
void RpcCall::Call(Meta& meta, std::vector<uint8_t>& data)
{
    if (!session_)
        return;
        session_->Send(&meta, data);
}


bool RpcCall::HasCallBack()
{
    return done_call_bcak_ != nullptr;
}

bool RpcCall::HasSession()
{
    if (!session_)
        return false;
    if (session_->is_closed() || session_->is_connected())
        return false;
    return true;
}

void RpcCall::Done(const SessionPtr& session, const ReadBufferPtr& buffer, Meta& meta, int meta_size, int64_t data_size) const
{
    done_call_bcak_(session,buffer,meta, meta_size,data_size);
}

} // namespace gb
