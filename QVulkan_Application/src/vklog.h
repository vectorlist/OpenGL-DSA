#pragma once

#include <vulkan/vulkan.h>
#include <Windows.h>
#include <vector>
#include <io.h>
#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>

//#define USE_QT true

#ifdef QT_CORE_LIB

#define USE_QT true
#include <qmessagebox.h>
#include <qapplication.h>

#else
#define USE_QT false

#endif

#define LOG std::cout
#define ENDL std::endl

#define SAFE_DELETE(x) if( x ) { delete (x); (x) = NULL; }
#define SAFE_DELETE_ARRAY(p)	do{  if( (p) != NULL ) { delete [] (p); (p)=NULL; } } while(0)

#define VK_OVERRIDE		override

#define LOG_ERROR(x) Log::proxylog(x)
#define LOG_RESULT(x) Log::proxylog.logResult(x)
#define LOG_ASSERT(x) Log::proxylog.logAssert(x)
#define LOG_SECTION(x) Log::proxylog.logSection(x)
#define LOG_WARN(x) Log::proxylog.logWarning(x)

#define CLASS_TYPE(x) Log::proxylog.getClassType(x)

class Log
{
public:
	static Log proxylog;

	Log& operator()(const std::string &msg)
	{
		m_msg = msg;
		return *this;
	}
	void operator<<(const VkResult &code)
	{
		if (code == VK_SUCCESS) return;

		if (USE_QT)
		{
			auto re = QMessageBox::warning(NULL, "debug error", m_msg.c_str());
			if (re == QMessageBox::Ok)
				qApp->quit();
		}
		else 
		{
			MessageBox(NULL, m_msg.c_str(), "debug error", MB_ICONSTOP);
		}
	}
	void logAssert(const std::string &msg)
	{
		if (USE_QT) {
			auto re = QMessageBox::warning(NULL, "assert error", msg.c_str());
			if (re == QMessageBox::Ok)
				qApp->quit();
			Q_ASSERT( 0 && msg.c_str());
		}
		else
		{
			MessageBox(NULL, msg.c_str(), "assert error", MB_ICONSTOP);
			assert(0 && msg.c_str());
			std::exit(1);
		}
	}
	void logWarning(const std::string &msg)
	{
		std::string bits = "warning : ";
		bits.append(msg);
		if (USE_QT)
			qWarning(bits.c_str());
		else
			LOG << bits.c_str() << ENDL;
	}

	void logResult(VkResult code)
	{
		if (code == VK_SUCCESS) return;
		/*if (USE_QT)
		{
			qFatal()
		}*/
	}

	void logSection(const std::string &msg);

	template<typename T>
	std::string getClassType(T object)
	{
		std::string tname = typeid(object).name();
		return tname;
	}
	std::string m_msg;
};

