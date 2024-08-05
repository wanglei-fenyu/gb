#include "MyApp.h"
#include "test.h"
#include "network.h"

static bool is_net_init = false;




static async_simple::coro::Lazy<void> test_coro_2(const gb::SessionPtr& session)
{
	gb::RpcCallPtr call = std::make_shared<gb::RpcCall>();
	call->SetSession(session);
    int str = co_await CoRpc<int>::execute(call, "square", 10000);
    LOG_INFO("CORO_TEST  {}", str);
    
    auto [a, b] = co_await CoRpc<int, std::string>::execute(call, "test_ret_args", 2, "world");
    LOG_INFO("CORO_TEST_2  {} {}", a, b);

}


int MyApp::OnInit()
{
	log.Init(ResPath::Instance()->FindResPath("log4/test.log").c_str(), 1024 * 1024 * 1000, 10,
		   Hlog::ASYNC, Hlog::CONSOLE_AND_FILE, Hlog::LEVEL_INFO);

    gb::ClientOptions options;
    options.keep_alive_time = -1;
    client_.reset(new gb::Client(options));
    Test_Register();

	client_->SetCloseCallBack([](const gb::SessionPtr session) {
        LOG_INFO("net close");
    });

	client_->SetConnnectCallBack([this](const gb::SessionPtr session) {
        session->set_return_io_service_pool_fun([&]()-> gb::IoServicePoolPtr {
            return client_->GetIoServicePool();
        });
        LOG_INFO("net connect");
        is_net_init = true;
        //session->StartHeartbeat(std::chrono::seconds(2));
    });
    
	client_->SetReceivedCallBack(OnReceived);

	auto [ip, port] = AppTypeMgr::Instance()->GetServerIpPort();
	std::string uir = ip + ":" + port;
    client_->Connect(gb::CONNECT_TYPE::CT_GATEWAY, uir);
    SetIoServicePool(client_->GetIoServicePool());
    test_http();
    return 0;
}

int MyApp::OnStartup(gb::WorkerPtr worker)
{
    if (worker)
        worker->OnStartup();
    return 0;
}

int MyApp::OnUpdate(gb::WorkerPtr worker)
{
    if (worker)
        worker->OnUpdate();
    return 0;
}

int MyApp::OnTick(gb::WorkerPtr worker, float elapsed)
{

    if (is_net_init)
    {
        is_net_init = false;
        //SendMsg1(client_);
        //SendRpc(client_);
        async_simple::coro::syncAwait(test_coro_2(client_->GetSession(gb::CONNECT_TYPE::CT_GATEWAY)));

    }
    if (worker)
        worker->OnTick(elapsed);
    return 0;

}

int MyApp::OnCleanup(gb::WorkerPtr worker)
{
    if (worker)
        worker->OnCleanup();
    return 0;
}

int MyApp::OnUnInit()
{
    client_->Shutdown();
    log.UnInit();
    return 0;
}

void MyApp::test_http()
{
    using namespace gb::http;
	auto [ip, port] = AppTypeMgr::Instance()->GetServerIpPort(UIR_TYPE::UT_WIN_HTTP);
    http_client     = std::make_shared<HttpClient>(ip + ":" + port);
    
    std::string json_string = "{\"firstName\": \"John\",\"lastName\": \"Smith\",\"age\": 25}";

  // Synchronous request examples
  try {
    //auto r1 =  http_client->request("GET", "/match/123");
      std::ostringstream os;
    //  os << r1->content.rdbuf();
    //  LOG_INFO("GET: {}", os.str());

    auto r2 = http_client->request("POST", "/string", json_string);
    os.clear();
    os << r2->content.rdbuf();
    LOG_INFO("POST: {}", os.str());
  }
  catch(const gb::http::system_error &e) {
    LOG_INFO("Client request error: {}", e.what());
  }
  http_client->io_service->run();

}
