/// @fn Logger.hpp
/// @author Shirao Shotaro
///
/// Copyright 2018 Shirao Shotaro
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files(the "Software"),
/// to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
/// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :
///
/// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
/// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once
#ifndef WLIB_LOGGER_HPP_
#define WLIB_LOGGER_HPP_

#include <string>
#include <iostream>

#if _MSC_VER >= 1700
// Microsoft Visual Studio 2012以上 
#define SRCINFO wlib::Logger::source_information(__FILE__, __FUNCTION__, __LINE__)
#elif __GNUC__ >= 5
// g++ ver5以上
#define SRCINFO wlib::Logger::source_information(__FILE__, __FUNCTION__, __LINE__)
#else
// その他不明な処理系
#define SRCINFO wlib::Logger::source_information("unknown", "unknown", -1)
#endif

namespace wlib {

/// @brief Logging class
///
/// You can use this log class like "std::cout" or "std::cerr".
/// There are 7 levels for logging.
/// Watch Level Description, and understand these means.
///
class Logger{
public:

	/// @brief Log Level
	enum Level : size_t {
		kTrace = 0,		///< infomation of detail
		kPerformance,	///< performance (memory, fps, ...) log
		kDebug,			///< for debugging log
		kInfo,			///< information which users should know.
		kWarning,		///< error is occuring, but there is no problem to continue the system.
		kError,			///< error is occuring, and the system is uncontinuable.
		kFatal,			///< fatal error is occuring, and fail the system.
		kLevelNum		///< Level Num.
	};

	/// @brief Log Destination
	enum Destination {
		kOut,	///< standard output
		kErr	///< standard error
	};

	///////////////////////////////////////////////////////////////////
	// Static function
	///////////////////////////////////////////////////////////////////
	/// @brief set cout stream redirection source.
	///
	/// This function is NOT threadsafe.
	/// If dst_level is kLevelNum, this function will do nothing.
	/// @param [in] dst_level level of redirecting distination
	static void setRedirectionCout(
		const Level dst_level
	);

	/// @brief set cerr stream redirection source.
	///
	/// This function is NOT threadsafe.
	/// If dst_level is kLevelNum, this function will do nothing.
	/// @param [in] dst_level level of redirecting distination
	static void setRedirectionCerr(
		const Level dst_level
	);

	/// @brief set Enable or Disable output each of destination.
	///
	/// This function is NOT threadsafe.
	/// @brief [in] stdout Set true, and stdout output is enabled. Set false, it is disabled.
	/// @brief [in] stderr Set true, and stderr output is enabled. Set false, it is disabled.
	static void setOutputEnabled(
		bool stdout_enable,
		bool stderr_enable
	);

	/// @brief set Enable or Disable output each of level.
	///
	/// This function is NOT threadsafe.
	static void setOutputEnabled(
		bool trace_enable,
		bool performance_enable,
		bool debug_enable,
		bool info_enable,
		bool warning_enable,
		bool error_enable,
		bool fatal_enable
	);

	/// @brief set all distination output enabled.
	///
	/// This function is NOT threadsafe.
	static void setOutputAllDistinationEnabled(void);

	/// @brief set all level output enabled.
	///
	/// This function is NOT threadsafe.
	static void setOutputAllLevelEnabled(void);

	/// @brief set log output destination.
	///
	/// This function is NOT threadsafe.
	/// @param [in] trace Trace log destination. Default is kOut
	/// @param [in] performance Performance log destination. Default is kOut.
	/// @param [in] debug Debug log destination. Default is kOut.
	/// @param [in] info Info log destination. Default is kOut.
	/// @param [in] warning Warning log destination. Default is kErr.
	/// @param [in] error Error log destination. Default is kErr.
	/// @param [in] fatal Fatal error log destination. Default is kErr.
	static void setDestination(
		const Destination trace = Destination::kOut,
		const Destination performance = Destination::kOut,
		const Destination debug = Destination::kOut,
		const Destination info = Destination::kOut,
		const Destination warning = Destination::kErr,
		const Destination error = Destination::kErr,
		const Destination fatal = Destination::kErr
	);

	/// @brief Generate string of source information.
	///
	/// Generate string "[Filepath]::[Function](line) " for debugging or noticing errors.
	static std::string source_information(const std::string file, const std::string func, const int line);

protected:
	Logger(void);
	void _print(const char buffer[], const Level level) const;

private:
};

/// @brief Extended of std::streambuf and Logger class.
///
/// This class is not important for you.
struct LoggerBuffer : public std::streambuf, public Logger {
	LoggerBuffer(const Logger::Level level);
	virtual ~LoggerBuffer();
private:
	const Logger::Level level;
	virtual int sync(void) override;
	static constexpr size_t kBufferSize = 2048;
	char buffer[kBufferSize];
};

/// @brief Extended of std::iostream.
///
/// This class is not important for you.
struct LoggerStream : public std::iostream {
	LoggerStream(const Logger::Level level);
	virtual ~LoggerStream() {}
private:
	LoggerBuffer logger;
};

extern LoggerStream trace;	///< Trace Log
extern LoggerStream perf;	///< Performance Log
extern LoggerStream debug;	///< Debugging Log
extern LoggerStream info;	///< Information Log
extern LoggerStream warn;	///< Warning Log
extern LoggerStream error;	///< Error Log
extern LoggerStream fatal;	///< Fatal Log

};

#endif
