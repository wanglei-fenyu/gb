#include "MyApp.h"
#include "network/network.h"
#include "test.h"

int MyApp::OnInit()
{
	log.Init(ResPath::Instance()->FindResPath("log4/test.log").c_str(), 1024 * 1024 * 1000, 10,
		   Hlog::ASYNC, Hlog::CONSOLE_AND_FILE, Hlog::LEVEL_INFO);

    gb::WorkerManager* work_mng = gb::WorkerManager::Instance(4);
    gb::net_init();
    init_http();
    Test_Register();
    return 0;
}

int MyApp::OnStartup(gb::WorkerPtr worker)
{
    if (worker)
        worker->Post([worker]() { worker->OnStartup();});
    return 0;
}

int MyApp::OnUpdate(gb::WorkerPtr worker)
{
    if (worker)
        worker->Post([worker]() { worker->OnUpdate(); });
    return 0;
}

int MyApp::OnTick(gb::WorkerPtr worker, float elapsed)
{
    if (worker)
        worker->Post([worker,elapsed]() { worker->OnTick(elapsed); });
    return 0;

}

int MyApp::OnCleanup(gb::WorkerPtr worker)
{
    if (worker)
        worker->Post([worker]() { worker->OnCleanup(); });
    return 0;
}

int MyApp::OnUnInit()
{
    log.UnInit();
    http_server->stop();
    http_thread.join();
    return 0;
}

void MyApp::init_http()
{
    using namespace gb::http;
    http_server = std::make_shared<Server<HTTP>>();
	auto [ip, port] = AppTypeMgr::Instance()->GetServerIpPort(UIR_TYPE::UT_WIN_HTTP);
    http_server->config.port = std::atoi(port.c_str());
    http_server->config.address = ip;

    http_server->on_error = [](std::shared_ptr<HttpServer::Request> request, const error_code& ec) {
        //LOG_ERROR("http error:{}",ec.message());
    };

    http_server->resource["^/string$"]["POST"] = [](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
        std::string content = request->content.string();
        response->write(content);
   /*     *response << "HTTP/1.1 200 OK\r\nContent-Length: " 
                  << content.length() 
                  << "\r\n\r\n"
                  << content;*/
    };
    http_server->resource["^/info$"]["GET"] = [](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
		std::stringstream stream;
		stream << "<h1>Request from " << request->remote_endpoint_address() << ":" << request->remote_endpoint_port() << "</h1>";

		stream << request->method << " " << request->path << " HTTP/" << request->http_version;

		stream << "<h2>Query Fields</h2>";
		auto query_fields = request->parse_query_string();
		for(auto &field : query_fields)
		  stream << field.first << ": " << field.second << "<br>";

		stream << "<h2>Header Fields</h2>";
		for(auto &field : request->header)
		  stream << field.first << ": " << field.second << "<br>";
		response->write(stream);
  };
    
    http_thread = std::thread([&]() {
        http_server->start();
    });
}
