#include "test.h"


void hello(const std::shared_ptr<gb::Session>& session)
{
	LOG_INFO("Hello")
}

void World(const std::shared_ptr<gb::Session>& session,TestMsg& msg)
{
	Meta meta;
	meta.set_id(2);
	meta.set_type(1);
	session->Send(&meta,&msg);
	LOG_INFO("wrold {}",msg.msg())
}

void test_rpc()
{
	LOG_INFO("test_rpc")
}



void test_rpc2(int a)
{
	LOG_INFO("test_rpc {}",a)
}


void square(gb::RpcReply reply, int a)
{
	reply.Invoke(a*a);
}

void test_ret_args(gb::RpcReply reply,int a,std::string b)
{
    LOG_INFO("{}:{}", a, b);
    reply.GetMeta().set_compress_type(CompressTypeNone);
    reply.Invoke(a*2, b+" hello");
}

void SessionMsg(const gb::SessionPtr& session,TestMsg& msg)
{
	LOG_INFO("Session")
}


void Test_Register(gb::IoServicePoolPtr pool)
{
			
	pool->Post([]()->void{
		//gb::Listen(1, 2, World);
		gb::Register("test_rpc", test_rpc);
		gb::Register("test_rpc2", test_rpc2);
		gb::Register("square", square);
		gb::Register("test_ret_args", test_ret_args);

    });
}
