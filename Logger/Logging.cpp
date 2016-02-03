#include "StdAfx.h"
#include "Logging.h"

#include <log4cpp/Category.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/Priority.hh>


Logging::Logging(const std::string& strLogInstanceName, const std::string& strLogFilePathName)
: m_strLogInstanceName(strLogInstanceName)
, m_strLogFilePathName(strLogFilePathName)
{
	log4cpp::Appender *appender1 = new log4cpp::OstreamAppender("console", &std::cerr);
	appender1->setLayout(new log4cpp::BasicLayout());

	log4cpp::Appender *appender2 = new log4cpp::FileAppender(strLogInstanceName, strLogFilePathName);
	appender2->setLayout(new log4cpp::BasicLayout());

	log4cpp::Category& root = log4cpp::Category::getRoot();
	root.setPriority(log4cpp::Priority::WARN);
	root.addAppender(appender1);

	log4cpp::Category& sub1 = log4cpp::Category::getInstance(strLogInstanceName);
	sub1.setPriority(log4cpp::Priority::DEBUG);
	sub1.addAppender(appender2);

	//// use of functions for logging messages
	//root.fatal("error");
	//root.error("root error");
	//root.info("root info");
	//sub1.error("sub1 error");
	//sub1.warn("sub1 warn");
	//sub1.debug("debug");

	//// printf-style for logging variables
	//root.warn("%d + %d == %s ?", 1, 1, "two");

	//// use of streams for logging messages
	//root << log4cpp::Priority::ERROR << "Streamed root error";
	//root << log4cpp::Priority::INFO << "Streamed root info";
	//sub1 << log4cpp::Priority::ERROR << "Streamed sub1 error";
	//sub1 << log4cpp::Priority::WARN << "Streamed sub1 warn";

	//// or this way:
	//root.errorStream() << "Another streamed error";
}

Logging::~Logging(void)
{
	log4cpp::Category& root = log4cpp::Category::getRoot();
	root.removeAllAppenders();

	if(log4cpp::Category* pLogInst = log4cpp::Category::exists(m_strLogInstanceName))
	{
		pLogInst->removeAllAppenders();
	}
}

void Logging::log(Logging::LOG_LEVEL level, const char* stringFormat, ...)
{
	va_list list;
	va_start(list, stringFormat);

	if(log4cpp::Category* pLogInst = log4cpp::Category::exists(m_strLogInstanceName))
	{
		//log4cpp::Category& output = log4cpp::Category::getInstance(m_strLogInstanceName);
		log4cpp::Category& output = (*pLogInst);
		bool bEnabled = false;
		log4cpp::Priority::Value ePriority = log4cpp::Priority::NOTSET;
		switch (level)
		{
		case L_FATAL:
			bEnabled = output.isFatalEnabled();
			ePriority = log4cpp::Priority::FATAL;
			break;
		case L_ERROR:
			bEnabled = output.isErrorEnabled();
			ePriority = log4cpp::Priority::ERROR;
			break;
		case L_WARN:
			bEnabled = output.isWarnEnabled();
			ePriority = log4cpp::Priority::WARN;
			break;
		case L_INFO:
			bEnabled = output.isInfoEnabled();
			ePriority = log4cpp::Priority::INFO;
			break;
		case L_DEBUG:
		default:
			bEnabled = output.isDebugEnabled();
			ePriority = log4cpp::Priority::DEBUG;
			break;
		}
		if(bEnabled)
		{
			output.logva(ePriority, stringFormat, list);
		}
	}
	else
	{
		//log4cpp::Category& root = log4cpp::Category::getRoot();
	}
}

void Logging::setLogLevel( LOG_LEVEL level )
{
	if(log4cpp::Category* pLogInst = log4cpp::Category::exists(m_strLogInstanceName))
	{
		log4cpp::Priority::Value ePriority = log4cpp::Priority::NOTSET;
		switch (level)
		{
		case L_FATAL:
			ePriority = log4cpp::Priority::FATAL;
			break;
		case L_ERROR:
			ePriority = log4cpp::Priority::ERROR;
			break;
		case L_WARN:
			ePriority = log4cpp::Priority::WARN;
			break;
		case L_INFO:
			ePriority = log4cpp::Priority::INFO;
			break;
		case L_DEBUG:
		default:
			ePriority = log4cpp::Priority::DEBUG;
			break;
		}
		pLogInst->setPriority(ePriority);
	}
}

//////////////////////////////////////////////////////////////////////////

static Logging* volatile g_Logger = NULL;

Logging* CreateLoggerInstance( const char* const strLogInstanceName, const char* const strLogFilePathName )
{
	if(!g_Logger)
	{
		Sleep(0);
		if(!g_Logger)
		{
			g_Logger = new Logging(strLogInstanceName, strLogFilePathName);
		}
	}
	ASSERT(g_Logger);
	return g_Logger;
}

Logging* GetLoggerInstance()
{
	ASSERT(g_Logger);
	return g_Logger;
}

void DestroyLoggerInstance( Logging* pLoggerInstance )
{
	if(pLoggerInstance)
	{
		delete pLoggerInstance;
	}
	else
	{
		Logging* p = GetLoggerInstance();
		if(p)
		{
			delete p;
			p = NULL;
		}
	}
}

void SetLoggerOutputLevel( Logging* pLoggerInstance, Logging::LOG_LEVEL level )
{
	Logging* p = pLoggerInstance;
	if(!p)	p = GetLoggerInstance();
	if(p)
	{
		p->setLogLevel(level);
	}
}
