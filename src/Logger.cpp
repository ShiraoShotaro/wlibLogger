#define _CRT_SECURE_NO_WARNINGS
#include "Logger.hpp"
#include <mutex>
#include <memory>
#include <iomanip>
#include <thread>
#include <sstream>
#include <chrono>
#include <ctime>
#include <cstdio>

namespace {
std::mutex _mutex;

// Logging level
#ifdef _DEBUG
constexpr wlib::Logger::Level kShowLogLevel = wlib::Logger::kTrace;
#else
constexpr wlib::Logger::Level kShowLogLevel = wlib::Logger::kInfo;
#endif

// String Coloring
#if defined(__unix__) || defined(__linux__)
std::string changeColorToGreen(void) { return "\033[31m"; }
std::string changeColorToRed(void) { return "\033[31m"; }
std::string changeColorToYellow(void) { return "\033[34m"; }
std::string changeColorToFatal(void) { return "\033[33m"; }
std::string resetColorStr(void) { return "\033[0m"; }
void resetColorPrc(void) {}
#elif defined(_WIN64) || defined(_WIN32)
#include <windows.h>
std::string changeColorToGreen(void) { SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY); return ""; }
std::string changeColorToRed(void) { SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY); return ""; }
std::string changeColorToYellow(void) { SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY); return ""; }
std::string changeColorToFatal(void) { SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_RED | BACKGROUND_INTENSITY); return ""; }
std::string resetColorStr(void) { return ""; }
void resetColorPrc(void){ SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); }
#endif
// redirected stream
std::streambuf * _cout_buf = nullptr;
std::streambuf * _cerr_buf = nullptr;
auto _cout_deleter = [](wlib::LoggerStream * ptr)
{ if(_cout_buf != nullptr) std::cout.rdbuf(_cout_buf); _cout_buf = nullptr; };
auto _cerr_deleter = [](wlib::LoggerStream * ptr)
{ if(_cerr_buf != nullptr) std::cerr.rdbuf(_cerr_buf); _cerr_buf = nullptr; };
std::unique_ptr<wlib::LoggerStream, decltype(_cout_deleter)> _cout;
std::unique_ptr<wlib::LoggerStream, decltype(_cerr_deleter)> _cerr;
};

// Extern variable instances
wlib::LoggerStream wlib::trace(wlib::Logger::kTrace);
wlib::LoggerStream wlib::perf(wlib::Logger::kPerformance);
wlib::LoggerStream wlib::debug(wlib::Logger::kDebug);
wlib::LoggerStream wlib::info(wlib::Logger::kInfo);
wlib::LoggerStream wlib::warn(wlib::Logger::kWarning);
wlib::LoggerStream wlib::error(wlib::Logger::kError);
wlib::LoggerStream wlib::fatal(wlib::Logger::kFatal);

void wlib::Logger::setRedirectionCout(const Level dst_level){
	if (dst_level != kLevelNum) {
		_cout = std::move(std::unique_ptr<LoggerStream, decltype(_cout_deleter)>
			(new LoggerStream(dst_level), std::move(_cout_deleter)));
		_cout_buf = std::cout.rdbuf(_cout->rdbuf());
	}
}

void wlib::Logger::setRedirectionCerr(const Level dst_level){
	if (dst_level != kLevelNum) {
		_cerr = std::move(std::unique_ptr<LoggerStream, decltype(_cerr_deleter)>
			(new LoggerStream(dst_level), std::move(_cerr_deleter)));
		_cerr_buf = std::cerr.rdbuf(_cerr->rdbuf());
	}
}

void wlib::Logger::setDestination(const Destination trace, const Destination performance, const Destination debug, const Destination info, const Destination warning, const Destination error, const Destination fatal)
{
}

std::string wlib::Logger::source_information(const std::string file, const std::string func, const int line)
{ return std::string(file + "::" + func + "(" + std::to_string(line) + ")"); }

wlib::Logger::Logger(void) : _distinations({kOut, kOut, kOut, kOut, kErr, kErr, kErr}) {}
void wlib::Logger::_print(const char buffer[], const Level level) const{
	//[STAT] 2017-10-15 03:47:24 Th:[thread_no] [filename.cpp]::[function]([line])

	//error check
	std::string buffer_str(buffer);
	if (buffer_str.size() == 0) return;

	//log level
	if (static_cast<size_t>(level) < static_cast<size_t>(kShowLogLevel)) return;
	
	//get the now date and time
	const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	const std::tm * local_now(std::localtime(&now));
	if (!local_now) {
		std::cerr << "failed to malloc in localtime function.";
		return;
	}

	//formatting status string
	std::stringstream output_text_sstream;
	std::string reset_color_str;

	//status text
	switch (level) {
	case kTrace:		output_text_sstream << "TRACE"; break;
	case kDebug:		output_text_sstream << "DEBUG"; break;
	case kPerformance:	output_text_sstream << "PERF "; break;
	case kInfo:			output_text_sstream << changeColorToGreen() << "INFO "; reset_color_str = resetColorStr(); break;
	case kWarning:		output_text_sstream << changeColorToYellow() << "WARN "; reset_color_str = resetColorStr(); break;
	case kError:		output_text_sstream << changeColorToRed() << "ERROR"; reset_color_str = resetColorStr(); break;
	case kFatal:		output_text_sstream << changeColorToFatal() << "FATAL"; reset_color_str = resetColorStr(); break;
	default: return;
	}

	output_text_sstream << " " << std::put_time(local_now, "%F %T") << " Th:" << std::this_thread::get_id();
	output_text_sstream << " | " << buffer_str;

	//for thread safe, lock with mutex
	{
		std::lock_guard<std::mutex> lock(_mutex);
		
		//write down to standard io
		output_text_sstream << reset_color_str;

		if (this->_distinations.at(level) == kOut) std::fprintf(stdout, "%s", output_text_sstream.str().c_str());
		else std::fprintf(stderr, "%s", output_text_sstream.str().c_str());
	}

	resetColorPrc();
}

// ======== Stream ========
wlib::LoggerBuffer::LoggerBuffer(const Logger::Level _level)
	: Logger(), level(_level) { setp(this->buffer, this->buffer + kBufferSize - 2); setg(this->buffer, this->buffer, this->buffer + kBufferSize - 2); }
wlib::LoggerBuffer::~LoggerBuffer(){}
int wlib::LoggerBuffer::sync(void){ 
	*pptr() = '\0';
	this->_print(this->buffer, this->level);
	pbump(static_cast<int>(pbase() - pptr()));
	return 0;
}
wlib::LoggerStream::LoggerStream(const Logger::Level _level)
	: std::iostream(&logger), logger(_level)
{}
