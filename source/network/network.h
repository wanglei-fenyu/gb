#pragma once 
#include "common/def.h"
#include "session/session.h"
#include "network_function.hpp"
#include "../log/log_help.h"

#include "rpc/rpc_call.h"
#include "rpc/rpc_reply.h"
#include "rpc/rpc_function.hpp"
#include "protobuf/meta.pb.h"
#include "md5.hpp"
#include "async_simple/coro/Lazy.h"
#include "async_simple/coro/SyncAwait.h"
#include "async_simple/executors/SimpleExecutor.h"
#include "io_service_pool/io_service_pool.h"

NAMESPACE_BEGIN(gb)


void                 net_init();
IoServicePoolPtr net_get_io_service_pool();

void Send(Session *session,int type, int id, google::protobuf::Message& msg);
void Send(std::shared_ptr<Session> session,int type, int id, google::protobuf::Message& msg);
void ListenOption(int type, int id, net_listen_fun f, std::string protoName);

//template<typename T>
//concept IsFunction = std::is_function_v<T>;

template <typename F>
void Listen(int type, int id, F f,std::string protoName="")
{
	net_listen_fun func;
	if  constexpr (std::is_same<F, sol::function>::value)
		func = NetFunctionaTraits<sol::function>::make(f, protoName);
	else if constexpr (HasInvokeOperator<typename std::decay<F>::type>::value)
		func = ServerLambdaFunc(f, &F::operator());
	else
		func = NetFunctionaTraits<F>::make(f);
	ListenOption(type, id, std::move(func), protoName);

}

void UnListen(int type, int id, std::string signal, int level = 0);
void Dispatch(const SessionPtr& session, const ReadBufferPtr& buffer, Meta& meta, int meta_size, int64_t data_size);

//void RegisterMessage();

uint64_t GetSequence();
void _Call(Meta& meta, RpcCall call);
void _Call(Meta& meta, RpcCall call, std::vector<uint8_t>& data);
void _Call(Meta& meta, RpcCall call, std::vector<uint8_t>&& data);
void _Call(RpcCall call, std::string method, sol::variadic_args& args);

template<typename ...Args>
void Call( RpcCall call,std::string method,Args&& ...args)
{
	Meta meta;
	uint64_t method_key = MD5::MD5Hash64(method.c_str());
	meta.set_method(method_key);
	uint64_t seq_id = GetSequence();
	meta.set_sequence(seq_id);
	meta.set_mode(MsgMode::Request);
	call.SetId(seq_id);
	if constexpr (sizeof...(args) > 0)
	{
		std::vector<uint8_t> data =  gb::msgpack::pack<Args...>(std::forward<Args>(args)...);
		_Call(meta, call,std::move(data));
	}
	else
	{
		_Call(meta, call);
	}
}

void RegisterOption(std::string method, rpc_listen_fun fn);
void UnRegister(std::string method);

template<typename F>
void Register(std::string method, F fn)
{
    net_listen_fun func;
	if  constexpr (!std::is_same<F, sol::function>::value)
		func = RpcFunctionaTraits<F>::make(fn);
	else if constexpr (HasInvokeOperator<typename std::decay<F>::type>::value)
		func = RpcLambdaFunc(fn, &F::operator());
	else
	{
		auto lua_state = fn.lua_state();
		sol::state_view lua_view(lua_state);
		sol::state* state = (sol::state*)&lua_view;
		func = RpcFunctionaTraits<sol::function>::make(state,fn);
	}
	RegisterOption(method, func);

}



template<typename T>
struct RpcCallAwaiter 
{
public:
	using private_call = std::function<void(std::coroutine_handle<>,T&)>;
public:
	RpcCallAwaiter(private_call call_back) : private_call_(std::move(call_back))
	{
	}
public:
	bool await_ready() noexcept { return false; }

	void await_suspend(std::coroutine_handle<> handle)
	{
		private_call_(handle,resoult_);
	}

	T await_resume() { return std::move(resoult_); }
private:
	T resoult_;
	private_call private_call_;
};

template<>
struct RpcCallAwaiter<void>
{
public:
	using private_call = std::function<void(std::coroutine_handle<>)>;
public:
	RpcCallAwaiter(private_call call_back) : private_call_(std::move(call_back))
	{
	}
public:
	bool await_ready() noexcept { return false; }

	void await_suspend(std::coroutine_handle<> handle)
	{
		private_call_(handle);
	}

	void await_resume() { return; }
private:
	private_call private_call_;
};

template <typename... Args>
void ProcessTuple(std::tuple<Args...>& tup)
{

};


template<typename T>
struct is_tuple : public std::false_type
{
};

template <typename... Args>
struct is_tuple<std::tuple<Args...>> : public std::true_type
{
};

template <typename T>
constexpr bool IsTuple()
{
    return is_tuple<std::decay_t<T>>::value;
}

template<typename T>
struct CallTuple
{
	static void SetCallBack(RpcCall& call, std::coroutine_handle<> h,T& tuple)
	{

	}
};

template<typename... Args>
struct CallTuple<std::tuple<Args...>>
{
	static void SetCallBack(RpcCall& call, std::coroutine_handle<> h,std::tuple<Args...>& tup)
	{
        call.SetCallBack([h,&tup](Args... args) {
            tup = std::forward_as_tuple(std::forward<Args>(args)...);
            h.resume();
        });
	}
};

template<typename T,typename... Args>
async_simple::coro::Lazy<T> CoRpcCall(RpcCall& call, std::string method, Args... args) noexcept
{
	if constexpr (std::is_void<T>::value)
	{
		co_return co_await RpcCallAwaiter<void>{[&](std::coroutine_handle<> h) {
			call.SetCallBack([&,h]() { h.resume(); });
			gb::Call(call, method, args...);
			}
		};
	}
	else if constexpr (IsTuple<T>())
	{
		co_return co_await RpcCallAwaiter<T>{[&](std::coroutine_handle<> h, T& resoult) {
            CallTuple<T>::SetCallBack(call, h, resoult);
			gb::Call(call, method, args...);
			}
		};
	}
	else
	{
		co_return co_await RpcCallAwaiter<T>{[&](std::coroutine_handle<> h, T& resoult) {
			call.SetCallBack([&,h](T t) { resoult = std::move(t); h.resume(); });
			gb::Call(call, method, args...);
			}
		};
	}
}



template<typename... Rets>
struct CoRpcImpl
{
	using ResultType = std::conditional_t<sizeof...(Rets) == 1, typename std::tuple_element<0, std::tuple<Rets...>>::type, std::tuple<Rets...>>;

	template<typename... Args>
    static async_simple::coro::Lazy<ResultType> execute(RpcCall& call, std::string method, Args... args) noexcept {
		co_return co_await RpcCallAwaiter<ResultType>{[&](std::coroutine_handle<> h, ResultType& result) {
			call.SetCallBack([&, h](ResultType r) { 
				result = std::move(r); 
				h.resume(); 
			});
			gb::Call(call, method, args...);
		}};
    }
};

//默认返回值类型为void
template<>
struct CoRpcImpl<void>
{
	template<typename... Args>
	static async_simple::coro::Lazy<void> execute(RpcCall& call, std::string method, Args... args) noexcept {
		co_return co_await RpcCallAwaiter<void>{[&](std::coroutine_handle<> h) {
			call.SetCallBack([&,h]() { h.resume(); });
			gb::Call(call, method, args...);
			}
		};
	}
};



template <typename... Rets>
struct CoRpcSelector
{
private:
    // Helper for single type case
    template <typename T>
    using SingleTypeSelector = std::conditional_t<
        std::is_void_v<T>,
        CoRpcImpl<void>,
        CoRpcImpl<T>>;

public:
    using type = std::conditional_t<
        (sizeof...(Rets) > 1),
        CoRpcImpl<Rets...>,
        SingleTypeSelector<typename std::tuple_element<0, std::tuple<Rets...>>::type>>;
};

template<typename... Rets>
using CoRpc = typename CoRpcSelector<Rets...>::type;




NAMESPACE_END

