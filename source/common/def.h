#pragma once 
#define NAMESPACE_BEGIN(name) namespace name {
#define NAMESPACE_END }

#if USE_STANDALONE_ASIO
#include <asio/asio.hpp>
namespace Asio = asio;
namespace ec   = asio;
#else
#include<boost/asio.hpp>
namespace Asio = boost::asio;
namespace ec   = boost::system;

#endif 

#define USE_MAIN_THREAD  false


#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif
#else
#include <limits.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#endif


