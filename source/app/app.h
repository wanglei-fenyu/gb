#pragma once
#include "../common/singleton.h"
#include "../common/app_type_mgr.h"
#include <memory>
#include <chrono>
#include <atomic>
#include "network/io_service_pool/io_service_pool.h"


class App {
public:
    App(APP_TYPE type) :
        appType_(type), runding_(false), frame_duration_(std::chrono::milliseconds(16)) {}
	virtual ~App() {}

	APP_TYPE GetAppType() { return appType_; }

    void   SetFrameRate(int fps) 
    {
        if (fps > 0)
        {
            frame_duration_ = std::chrono::milliseconds(1000 / fps);
        }
    }


    void SetIoServicePool(gb::IoServicePoolPtr io_service_pool)
    {
        io_service_pool_ = io_service_pool;
    }
    
    gb::IoServicePoolPtr& GetIoServicePool()
    {
        return io_service_pool_;
    }

    int Init()
    {
        if (OnInit() != 0) return -1;
        runding_ = true;
        return 0;
    }

    void Stop()
    {
        runding_ = false;
    }

    void Run()
    {
        if (!io_service_pool_)
        {
            return;
        }

        for (auto& work : io_service_pool_->Workers())
        {
			if (OnStartup(work) != 0)
			{
				LOG_INFO("OnStartup fail: worker id{}", work->GetWorkerId());
				return;
			}
        }
        
        auto last_time = std::chrono::high_resolution_clock::now();
        while (runding_)
        {
            auto current_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> elapsed = current_time - last_time;
            last_time = current_time;

			for (auto& work : io_service_pool_->Workers())
			{
                if (OnUpdate(work) != 0)
                {
                    break;
                }

                if (OnTick(work, elapsed.count()) != 0)
                {
                    break;
                }
			}
            
            auto frame_end_time = std::chrono::high_resolution_clock::now();
            auto frame_time = frame_end_time - current_time;

            if (frame_time < frame_duration_) {
                std::this_thread::sleep_for(frame_duration_ - frame_time);
            }

        }

    
        for (auto& work : io_service_pool_->Workers())
        {
            if (OnCleanup(work) != 0)
			{
				LOG_INFO("OnCleanup fail: worker id{}", work->GetWorkerId());
				return;
			}
        }
        OnUnInit();

    }

protected:
	virtual int OnInit() = 0;
    virtual int OnStartup(gb::WorkerPtr) = 0;
    virtual int OnUpdate(gb::WorkerPtr) = 0;
    virtual int OnTick(gb::WorkerPtr, float) = 0;
    virtual int OnCleanup(gb::WorkerPtr) = 0;
    virtual int OnUnInit() = 0;

private:
    gb::IoServicePoolPtr      io_service_pool_;
	APP_TYPE appType_;
    std::atomic<bool> runding_;
    std::chrono::milliseconds frame_duration_;
};
