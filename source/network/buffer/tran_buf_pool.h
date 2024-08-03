#pragma once
#include <memory>
#include "../common_component/atomic.h"

#ifndef TRAN_BUF_BLOCK_BASE_SIZE
// base_block_size = 64B
#define TRAN_BUF_BLOCK_BASE_SIZE (64u)
#endif

#ifndef TRAN_BUF_BLOCK_MAX_FACTOR
// max_block_size = 64<<9 = 32K
#define TRAN_BUF_BLOCK_MAX_FACTOR (9u)
#endif

/// <summary>
/// 线程安全
/// 引用计数转换池。
/// int 和 atomic<int>  大小相同
/// Block = BlockSize（int） + RefCount(atomic<int>) + Data
/// </summary>


namespace gb
{


class TranBufPool
{
public:
    inline static void* malloc(int factor = 0)
    {
        void* p = ::malloc(TRAN_BUF_BLOCK_BASE_SIZE << factor);
        if (p != nullptr)
        {
            *(reinterpret_cast<int*>(p)) = TRAN_BUF_BLOCK_BASE_SIZE << factor; //内存开头存放内存块的大小   存放在内存第一个int位
            //*(reinterpret_cast<int*>(p) + 1) = 1;                                      //申请出来后引用计数初始化为1  存放在内存的第二个int位
            auto refCount = reinterpret_cast<std::atomic<int>*>(p) + 1;
            *refCount     = 1;
            p             = reinterpret_cast<int*>(p) + 2; //前两个int位分别被 内存的大小和引用计数占用  真正数据位置在偏移两个int后开始
        }
        return p;
    }


    inline static int block_size(void* p)
    {
        return *(reinterpret_cast<int*>(p) - 2);
    }

    inline static int capacity(void* p)
    {
        //内存开头存放的容量 - 大小和引用计数
        return *(reinterpret_cast<int*>(p) - 2) - sizeof(int) * 2;
    }

    inline static void add_ref(void* p)
    {
        auto refCount = reinterpret_cast<std::atomic<int>*>(p) - 1;
        //refCount->fetch_add(1, std::memory_order_relaxed);
        atomic_inc(refCount);
    }

    inline static void free(void* p)
    {
        auto refCount = reinterpret_cast<std::atomic<int>*>(p) - 1;
        if (atomic_dec_ret_old(refCount) == 1)
        {
            ::free(reinterpret_cast<int*>(p) - 2);
        }
        //if (atomic_dec_ret_old(reinterpret_cast<int*>(p) - 1) == 1)
        //{
        //    ::free(reinterpret_cast<int*>(p) - 2);
        //}
    }
};

} // namespace gb
