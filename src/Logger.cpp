#define _CRT_SECURE_NO_WARNINGS
#include "Logger.hpp"
#include <mutex>
#include <iomanip>
#include <thread>
#include <sstream>
#include <chrono>
#include <ctime>

namespace {
std::mutex _mutex;

// Logging level
#ifdef _DEBUG
constexpr wlib::Logger::Level kDefaultLogLevel = wlib::Logger::kTrace;
#else
constexpr wlib::Logger::Level kDefaultLogLevel = wlib::Logger::kInfo;
#endif

// String Coloring
#if defined(__unix__) || defined(__linux__)
std::string changeColorToRed(void) { return "\033[31m"; }
std::string changeColorToYellow(void) { return "\033[33m"; }
std::string resetColorStr(void) { return "\033[0m"; }
void resetColorPrc(void) {}
#elif defined(_WIN64) || defined(_WIN32)
#include <windows.h>
std::string changeColorToRed(void){ SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY); return ""; }
std::string changeColorToYellow(void) { SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY); return ""; }
std::string resetColorStr(void) { return ""; }
void resetColorPrc(void){ SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); }
#endif

};

void wlib::Logger::setRedirectionCout(const Level dst_level)
{
}

void wlib::Logger::setRedirectionCerr(const Level dst_level)
{
}

void wlib::Logger::setDestination(const Destination trace, const Destination performance, const Destination debug, const Destination info, const Destination warning, const Destination error, const Destination fatal)
{
}

// Constructor(nop)
wlib::Logger::Logger(void){}

void wlib::Logger::_printTrace(const char buffer[]) const
{
}

void wlib::Logger::_printPerformance(const char buffer[]) const
{
}

void wlib::Logger::_printDebug(const char buffer[]) const
{
}

void wlib::Logger::_printInformation(const char buffer[]) const
{
}

void wlib::Logger::_printWarning(const char buffer[]) const
{
}

void wlib::Logger::_printError(const char buffer[]) const
{
}

void wlib::Logger::_printFatal(const char buffer[]) const
{
}

void wlib::Logger::_print(const char buffer[], const Level level) const
{
}

// INFORMATION
void wlib::Logger::information(const std::string & text) const{
	this->_write(kInfomation, text);
}

// WARNING
void wlib::Logger::warning(const std::string & text) const{
	this->_write(kWarning, text);
}

// ERROR
void wlib::Logger::error(const std::string & text) const{
	this->_write(kError, text);
}

// ABORT
void wlib::Logger::abort(const std::string & text) const{
	this->_write(kAbort, text);
}

// ログレベルの変更
void wlib::Logger::changeLogLevel(LogLevel log_level) {
	//for thread safe, lock with mutex
	std::lock_guard<std::mutex> lock(_mutex);
	this->log_level_ = log_level;
}

void wlib::Logger::_write(const LogStatus status, const std::string & text) const{
	//[STAT] 2017-10-15 03:47:24 Th:[thread_no] [filename.cpp]::[function]([line])
	//_____[text]
	//_____[multi text]
	//<br />

	//error check
	if (text.empty()) return;

	//log level
	if (static_cast<unsigned int>(status) < static_cast<unsigned int>(log_level_)) return;
	
	//multi line text split
	std::vector<std::string> multiline_text;
	std::string line_text("");
	std::stringstream ss{ text };
	while (std::getline(ss, line_text, '\n')) multiline_text.push_back(line_text);

	//get the now date and time
	const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	const std::tm * local_now(std::localtime(&now));
	if (!local_now) {
		std::cerr << "failed to malloc in localtime function.";
		return;
	}

	//formatting status string
	std::stringstream output_text_sstream;

	//status text
	switch (status) {
	case kAbort:
		output_text_sstream << changeColorToRed() << "ABRT"; break;
	case kError:
		output_text_sstream << changeColorToRed() << "ERRO"; break;
	case kWarning:
		output_text_sstream << changeColorToYellow() << "WARN"; break;
	case kInfomation:
	default:
		output_text_sstream << "INFO"; break;
	}

	// shorter filename
	std::string short_file_(file_);
	size_t fpos = short_file_.find_last_of("\\/");
	if (fpos != std::string::npos) short_file_ = short_file_.substr(fpos + 1);

	output_text_sstream << " " << std::put_time(local_now, "%F %T") << " Th:" << std::this_thread::get_id() << " " << short_file_ << "::" << func_ << "(" << line_ << ")";
	
	if (multiline_text.size() == 1) output_text_sstream << " " << *multiline_text.begin() << std::endl;
	else {
		output_text_sstream << std::endl;
		for (auto line_text_p = multiline_text.begin(); line_text_p != multiline_text.end(); ++line_text_p) {
			output_text_sstream << "     " << *line_text_p << std::endl;
		}
		output_text_sstream << std::endl;
	}

	//for thread safe, lock with mutex
	{
		std::lock_guard<std::mutex> lock(_mutex);
		
		//write down to standard io
		std::cerr << output_text_sstream.str() << resetColorStr();
	}

	resetColorPrc();

	if (status == kAbort) {
#if _MSC_VER >= 1700
		//Visual Studio 2012以上
		std::string serr = this->file_ + ":" + this->func_ + "(" + std::to_string(this->line_) + ") " + text;
		_RPT0(_CRT_ASSERT, serr);
#endif
		::abort();
	}

}

// ======== Stream ========
wlib::LoggerStreambuf::LoggerStreambuf(const Logger::LogStatus _status, const std::string file, const std::string func, const int line)
	: Logger(file, func, line) ,status(_status){
	setp(this->buffer, this->buffer + kBufferSize - 2);
	setg(this->buffer, this->buffer, this->buffer + kBufferSize - 2);
}

wlib::LoggerStreambuf::~LoggerStreambuf(){
	this->sync();
}

int wlib::LoggerStreambuf::sync(void){
	*pptr() = '\0';    // 終端文字を追加します。
	std::string temp(this->buffer);
	if(!temp.empty() && temp.find_first_not_of("\n") != std::string::npos) this->_write(status, temp);
	pbump(static_cast<int>(pbase() - pptr()));    // 書き込み位置をリセットします。
	return 0;
}
