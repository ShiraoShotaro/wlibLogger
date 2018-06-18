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

#include <array>
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
/// それぞれのインスタンスにタグ付けができ、また標準出力、ファイル出力の有効化、無効化ができます。
/// 初期化に関しては、changeLogLevel関数にてログレベルの変更を一番最初に適宜切り替えて下さい。ログレベルはkAll, kWarningAndError, kErrorOnly, kNoLoggingの４レベルで、それぞれすべて出力、警告及びエラー、エラーのみ、何も出力しない、となっています。この時の警告（Warning）は「続行可能なエラー」で、エラー（Error）は「続行不可能なエラー」の区分分けとしています。ログレベルはプログラムにおいて共通です。
/// 使い方は、まずコンストラクタを呼び出し、引数にてタグを指定します。また状況に応じて、標準出力、ファイル出力の有効、無効を切り替えて下さい。
/// そしてその後は、ログを出力したいタイミングで、information, warning, error関数を使用します。
/// 一番簡単な使い方は`Logger("Hogehoge").information("foobar");`とすることです。必ずしもすべての場合においてこの方法が最適とは限りません。
/// また、すべての関数、機能は、スレッドセーフが保証されています。
/// @version 2018.01
/// @since 2017.10
/// @author S.Shirao
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

	std::array<Destination, Level::kLevelNum> _distinations;
};

struct LoggerBuffer : public std::streambuf, public Logger {
	LoggerBuffer(const Logger::Level level);
	virtual ~LoggerBuffer();
private:
	const Logger::Level level;
	virtual int sync(void) override;
	static constexpr size_t kBufferSize = 2048;
	char buffer[kBufferSize];
};

struct LoggerStream : public std::iostream {
	LoggerStream(const Logger::Level level);
	virtual ~LoggerStream() {}
private:
	LoggerBuffer logger;
};

extern LoggerStream trace;
extern LoggerStream perf;
extern LoggerStream debug;
extern LoggerStream info;
extern LoggerStream warn;
extern LoggerStream error;
extern LoggerStream fatal;

};

#endif
