#pragma once
#include "network/network_function.hpp"
#include "network/rpc/rpc_call.h"
#include "network/md5.hpp"

void UnListen(int type, int id, std::string signal, int level);
void Listen_Option(int type, int id, gb::net_listen_fun f,std::string protoName);
template <typename F>
void Listen(int type, int id, F f, std::string protoName = "")
{
    gb::net_listen_fun func;
    if constexpr (std::is_same<F, sol::function>::value)
        func = gb::NetFunctionaTraits<sol::function>::make(f, protoName);
    else if constexpr (gb::HasInvokeOperator<typename std::decay<F>::type>::value)
        func = gb::ServerLambdaFunc(f, &F::operator());
    else
        func = gb::NetFunctionaTraits<F>::make(f);
    Listen_Option(type, id, std::move(func),protoName);
}

uint64_t GetSequence();
void     Call(Meta& meta, gb::RpcCall call);
void     Call(Meta& meta, gb::RpcCall call, std::vector<uint8_t>& data);
void     Call(Meta& meta, gb::RpcCall call, std::vector<uint8_t>&& data);
void     Call(gb::RpcCall call, std::string method, sol::variadic_args& args);

template <typename... Args>
void Call(gb::RpcCall call, std::string method, Args&&... args)
{
    Meta meta;
    uint64_t    method_key = MD5::MD5Hash64(method.c_str());
    meta.set_method(method_key);
    uint64_t seq_id = GetSequence();
    meta.set_sequence(seq_id);
    meta.set_compress_type(CompressTypeZlib);
    meta.set_mode(MsgMode::Request);
    call.SetId(seq_id);
    if constexpr (sizeof...(args) > 0)
    {
        std::vector<uint8_t> data = gb::msgpack::pack<Args&&...>(std::forward<Args>(args)...);
        Call(meta, call, data);
    }
    else
    {
        Call(meta, call);
    }
}

void OnReceived(const gb::SessionPtr& session, const gb::ReadBufferPtr& buffer, int meta_size, int64_t data_size);