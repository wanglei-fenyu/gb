#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/async_logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/details/thread_pool.h"
#include "spdlog/details/thread_pool-inl.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/async.h" //support for async logging
#include <stdarg.h>

//��־����
#define LOG_NAME "multi_sink"
//��װ�꣬û�иú��޷�����ļ������кŵ���Ϣ
#define LOG_TRACE(...) SPDLOG_LOGGER_CALL(spdlog::get(LOG_NAME), spdlog::level::trace, __VA_ARGS__)
#define LOG_DEBUG(...) SPDLOG_LOGGER_CALL(spdlog::get(LOG_NAME), spdlog::level::debug, __VA_ARGS__)
#define LOG_INFO(...) SPDLOG_LOGGER_CALL(spdlog::get(LOG_NAME), spdlog::level::info, __VA_ARGS__)
#define LOG_WARN(...) SPDLOG_LOGGER_CALL(spdlog::get(LOG_NAME), spdlog::level::warn, __VA_ARGS__)
#define LOG_ERROR(...) SPDLOG_LOGGER_CALL(spdlog::get(LOG_NAME), spdlog::level::err, __VA_ARGS__)
#define LOG_CRITI(...) SPDLOG_LOGGER_CALL(spdlog::get(LOG_NAME), spdlog::level::critical, __VA_ARGS__)

#define LOG_IF(condition,...) \
    !(condition) ? (void)0 : LOG_ERROR(__VA_ARGS__)

#define CHECK(expression) \
    LOG_IF(!(expression), "CHECK failed: " #expression)
#define CHECK_EQ(a, b) CHECK((a) == (b))
#define CHECK_NE(a, b) CHECK((a) != (b))
#define CHECK_LT(a, b) CHECK((a) <  (b))
#define CHECK_LE(a, b) CHECK((a) <= (b))
#define CHECK_GT(a, b) CHECK((a) >  (b))
#define CHECK_GE(a, b) CHECK((a) >= (b))


class Hlog
{
public:
	//��־���λ��
	enum OutPosition {
		CONSOLE = 0x01,	//����̨
		FILE = 0X02,	//�ļ�
		CONSOLE_AND_FILE = 0x03, //����̨+�ļ�
	};

	enum OutMode {
		SYNC,	//ͬ��ģʽ
		ASYNC,	//�첽ģʽ
	};

	//��־����ȼ�
	enum OutLevel {
		LEVEL_TRACE = 0,
		LEVEL_DEBUG = 1,
		LEVEL_INFO = 2,
		LEVEL_WARN = 3,
		LEVEL_ERROR = 4,
		LEVEL_CRITI = 5,
		LEVEL_OFF = 6,
	};

public:
	Hlog();
	~Hlog();

	/* func: ��ʼ����־ͨ��
	* @para[in] nFileName    : ��־�洢·��			��֧�����·���;���·����
	* @para[in] nMaxFileSize : ��־�ļ����洢��С	��Ĭ��1024*1024*10��
	* @para[in] nMaxFile     : ���洢���ٸ���־�ļ�	��Ĭ��10���������ֵ��ѭ�����ǣ�
	* @para[in] outMode      : ��־���ģʽ			��ͬ�����첽��
	* @para[in] outPos       : ��־���λ��			������̨���ļ�������̨+�ļ���
	* @para[in] outLevel     : ��־����ȼ�			��ֻ���>=�ȼ�����־��Ϣ��
	*/
	bool Init(const char* nFileName, const int nMaxFileSize = 1024 * 1024 * 10, const int nMaxFile = 10,
		const OutMode outMode = SYNC, const OutPosition outPos = CONSOLE_AND_FILE, const OutLevel outLevel = LEVEL_TRACE);
	void UnInit();

private:

public:
	std::shared_ptr<spdlog::logger> m_pLogger;
	bool m_bInit;
};
