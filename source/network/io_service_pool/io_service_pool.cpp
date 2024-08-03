#include "io_service_pool.h"
#include "log/log_help.h"
#include "script/register_script.h"
#include "common/res_path.h"
#include <memory>

namespace gb
{

Worker::Worker() :m_ioContextWork_(m_ioContext_), m_ioContextStrand_(m_ioContext_)
{
	m_scriptPtr_ = std::make_shared<Script>();
}

Worker::~Worker() 
{
	Stop();
}


void Worker::OnStart()
{
	//×¢²áÏûÏ¢¼àÌý

	//¼ÓÔØ½Å±¾
	using sol::lib;
	if (!m_scriptPtr_)
		return;
	m_scriptPtr_->open_libraries(lib::base, lib::package,lib::string,lib::table,lib::os,lib::bit32,lib::coroutine,lib::count,lib::debug,lib::ffi,lib::io,lib::jit,lib::math,lib::utf8);
	//×¢²á½Å±¾ 
	register_script(m_scriptPtr_);
	std::string scriptRootPath;

	scriptRootPath = ResPath::Instance()->FindResPath("script/main.lua");
	m_scriptPtr_->Load(scriptRootPath);
}


void Worker::Run(bool load_script)
{
    LOG_INFO("script_load is {}", load_script ? "true":"false");
	m_threadPtr_ = std::make_shared<std::thread>([load_script,this]() {
        if (load_script)
			OnStart();
        m_ioContext_.run();
		});
}

void Worker::Stop()
{
	m_ioContext_.stop();
    if (m_threadPtr_ && m_threadPtr_->joinable())
		m_threadPtr_->join();
}

int Worker::OnStartup()
{
    return 0;
}

int Worker::OnUpdate()
{
    return 0;
}

int Worker::OnTick(float elapsed)
{
    return 0;
}

int Worker::OnCleanup()
{

	return 0;
}

uint32_t Worker::GetWorkerId()
{
	if (!m_threadPtr_)
		return 0;
	auto id = m_threadPtr_->get_id();
	uint32_t thread_id = *((uint32_t*)&id);

	return thread_id;
}

IoServicePool::IoServicePool(int workerNum) :
    _next_service(0)
{
	for (int i = 0; i < workerNum; i++)
	{
		auto worker = WorkerPtr(new Worker());
		m_workers.push_back(worker);
		uint32_t id = worker->GetWorkerId();
		m_worker_map.insert({ id,worker });
	}
}

void IoServicePool::Stop()
{
	for (auto& worker : m_workers)
	{
		worker->Stop();
	}
}

std::optional<IoServicePool::WorkerPtr> IoServicePool::GetWorker(int index)
{
	if (index > GetWorkerCount() || index < 0)
		return std::nullopt;
	return m_workers.at(index);
}

std::optional<IoServicePool::WorkerPtr> IoServicePool::GetWorkerById(uint32_t id)
{
	auto worker_it = m_worker_map.find(id);
	if (worker_it == m_worker_map.end())
		return std::nullopt;
	return worker_it->second;
}

std::pair<int,IoService&> IoServicePool::GetIoService()
{
    int  cur_service = _next_service;
    IoService& ios = m_workers[_next_service]->GetIoContext();
    ++_next_service;
    if (_next_service == m_workers.size())
        _next_service = 0;
    return {cur_service, ios};
}

const std::vector<gb::IoServicePool::WorkerPtr>& IoServicePool::Workers() const
{
    return m_workers;
}

void IoServicePool::Run(bool load_script)
{
    for (const WorkerPtr& e:m_workers)
	{
        e->Run(load_script);
	}
}
}