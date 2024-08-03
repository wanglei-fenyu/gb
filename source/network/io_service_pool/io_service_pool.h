#pragma once 
#include "common/def.h"
#include <functional>
#include "script/script.h"
#include "common/singleton.h"
#include "../common_component/define.h"


namespace gb
{

template <typename F>
concept FUNC = requires () { std::is_function<F>::value; };

class Worker : public std::enable_shared_from_this<Worker>
{
	using ThreadPtr = std::shared_ptr<std::thread>;
	using ScriptPtr = std::shared_ptr<Script>;
public:
	Worker();
	virtual ~Worker();
public:
	void OnStart();
	void Run(bool load_script = true);
	void Stop();
	IoService& GetIoContext() { return m_ioContext_; }

public:
    virtual int OnStartup();
    virtual int OnUpdate();
    virtual int OnTick(float elapsed);
    virtual int OnCleanup();
public:
	template<FUNC F>
	void Post(F f);

	template<FUNC F> 	
	void Dispatch(F f);
public:
	ScriptPtr GetScript() { return m_scriptPtr_; }
	uint32_t GetWorkerId();

private:
	IoService			 m_ioContext_;
	IoServiceWork		 m_ioContextWork_;
	IoServiceStrand		 m_ioContextStrand_;
	
	ScriptPtr	 m_scriptPtr_;
	ThreadPtr    m_threadPtr_;
};


template<FUNC F>
void Worker::Post(F f)
{
	m_ioContextStrand_.post(f);
}


template<FUNC F>
void Worker::Dispatch(F f)
{
	m_ioContextStrand_.dispatch(f);
}


class IoServicePool 
{
	using WorkerPtr = std::shared_ptr<Worker>;
public:
    IoServicePool(int workerNum = 4);

public:
	size_t GetWorkerCount() { return m_workers.size(); }
	std::optional<WorkerPtr> GetWorker(int index);
	std::optional<WorkerPtr> GetWorkerById(uint32_t id);
	std::pair<int,IoService&> GetIoService();
    const std::vector<WorkerPtr>&    Workers() const;
	void Stop();
    void Run(bool load_script = true);

public:
	template<FUNC F>	
	void Post(F f);

	template<FUNC F>
	void Dispatch(F f);
private:
	std::vector<WorkerPtr> m_workers;
	std::map<uint32_t, WorkerPtr> m_worker_map;			//key Ïß³Ìid
    size_t                        _next_service;
};


template<FUNC F> 
void IoServicePool::Post(F f)
{
	std::for_each(m_workers.begin(), m_workers.end(), [f](WorkerPtr& workerPtr) {
		workerPtr->Post(f);
	});
}


template<FUNC F> 
void IoServicePool::Dispatch(F f)
{
	std::for_each(m_workers.begin(), m_workers.end(), [f](WorkerPtr& workerPtr) {
		workerPtr->Dispatch(f);
	});
}


using IoServicePoolPtr = std::shared_ptr<IoServicePool>;
using WorkerPtr = std::shared_ptr<Worker>;
}