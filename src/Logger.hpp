//////////////////////////////////////////////////////
// Logger.hpp
//
// MIT LICENSE
//////////////////////////////////////////////////////

#pragma once
#ifndef JUBEON_LOGGER_HPP_
#define JUBEON_LOGGER_HPP_

#include <string>
#include <iostream>

#if _MSC_VER >= 1700

// Microsoft Visual Studio 2012以上 
#define LOG_INFO  jubeon::LoggerInformation(__FILE__, __FUNCTION__, __LINE__)
#define LOG_WARN  jubeon::LoggerWarning(__FILE__, __FUNCTION__, __LINE__)
#define LOG_ERROR jubeon::LoggerError(__FILE__, __FUNCTION__, __LINE__)
#define LOG_ABORT jubeon::LoggerAbort(__FILE__, __FUNCTION__, __LINE__)

#elif __GNUC__ >= 5

// g++ ver5以上
#define LOG_INFO  jubeon::LoggerInformation(__FILE__, __func__, __LINE__)
#define LOG_WARN  jubeon::LoggerWarning(__FILE__, __func__, __LINE__)
#define LOG_ERROR jubeon::LoggerError(__FILE__, __func__, __LINE__)
#define LOG_ABORT jubeon::LoggerAbort(__FILE__, __func__, __LINE__)

#else

// その他不明な処理系
#define LOG_INFO  jubeon::LoggerInformation("unknown", "unknown", -1)
#define LOG_WARN  jubeon::LoggerWarning("unknown", "unknown", -1)
#define LOG_ERROR jubeon::LoggerError("unknown", "unknown", -1)
#define LOG_ABORT jubeon::LoggerAbort("unknown", "unknown", -1)

#endif


namespace jubeon {

/// @brief 非常に簡単なログクラスです。
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
class Logger {
public:

	/// @brief ログレベル定数
	///
	/// ログレベルはkAll, kWarningAndError, kErrorOnly, kNoLoggingの4レベルがあります。
	/// ただし、警告（Warning）は「続行可能なエラー」で、エラー（Error）は「続行不可能なエラー」と意味付けされています。
	/// 全体のログレベルをchangeLogLevel関数にて変更することができます。
	/// @see jubeon::Logger::changeLogLevel()
	enum LogLevel : unsigned int {
		kAll = 0,				///< 全て出力
		kWarningAndError = 1,	///< 警告及びエラー
		kErrorOnly = 2,			///< エラーのみ
		kNoLogging = 4			///< 何も出力しない
	};

	enum LogStatus : unsigned int {
		kInfomation = 0,
		kWarning = 1,
		kError = 2,
		kAbort = 3
	};
	
	/// @brief コンストラクタ
	///
	/// コンストラクタをコールするタイミングの指定はありません。インスタンスを保持する必要もありません。
	/// @param tag タグ。基本的にはクラス名を指定して下さい。
	/// @param is_output_stdio 標準出力へ出力するかどうか。デフォルトではtrueが指定されています。
	/// @param is_output_file ファイルへ出力するかどうか。デフォルトではtrueが指定されています。ファイル名の取得はgetLogFilename関数で可能です。
	/// @see getLogFilename()
	explicit Logger(const std::string file, const std::string func, const int line);

	/// @brief デストラクタ
	virtual ~Logger();

	/// @brief 「情報」として出力
	///
	/// 指定された文字列を、「情報」としてログ出力します。
	/// @param text ログ文字列
	void information(const std::string & text) const;

	/// @brief 「警告」として出力
	///
	/// 指定された文字列を、「警告」としてログ出力します。
	/// @param text ログ文字列
	void warning(const std::string & text) const;

	/// @brief 「エラー」として出力
	///
	/// 指定された文字列を、「エラー」としてログ出力します。
	/// @param text ログ文字列
	void error(const std::string & text) const;

	/// @brief 「Abort」として出力
	///
	/// この関数を呼出した場合、ログに書き出した後Abortがコールされるため、プログラムは強制的に停止します。
	/// @param text エラーメッセージはできるだけ詳しく記述してください
	void abort(const std::string & text) const;

	/// @brief ログレベルの変更
	///
	/// ログレベルはプロジェクトにて共有されています。ログレベルの変更後に、新たに出力されるログに対して、そのログレベルは適用されます。
	/// @param log_level 新しく指定するログレベル
	void changeLogLevel(LogLevel log_level);

protected:
	
	/// @brief 出力関数
	///
	/// @param status ステータス文字列。「INFO」や「WARN」、「ERRO」が格納されます。
	void _write(const LogStatus status, const std::string & text) const;

private:
	
	static LogLevel log_level_;	///< ログレベル格納用の変数
	Logger() = delete;
	Logger(const Logger &) = delete;
	Logger(Logger &&) = delete;

	const std::string file_, func_;
	const int line_;
};

namespace { static constexpr size_t kBufferSize = 2048; };

struct LoggerStreambuf : public std::streambuf, public Logger{
	LoggerStreambuf() = delete;
	LoggerStreambuf(const Logger::LogStatus status, const std::string file, const std::string func, const int line);
	virtual ~LoggerStreambuf();
private:
	virtual int sync(void) override;
	char buffer[kBufferSize];
	const Logger::LogStatus status;
};

struct LoggerInformation : public std::iostream {
	LoggerInformation() = delete;
	LoggerInformation(const std::string file, const std::string func, const int line) : sbuf(Logger::kInfomation, file, func, line), std::iostream(&sbuf) {}
private:
	LoggerStreambuf sbuf;
};

struct LoggerWarning: public std::iostream {
	LoggerWarning() = delete;
	LoggerWarning(const std::string file, const std::string func, const int line) : sbuf(Logger::kWarning, file, func, line), std::iostream(&sbuf) {}
private:
	LoggerStreambuf sbuf;
};

struct LoggerError : public std::iostream {
	LoggerError() = delete;
	LoggerError(const std::string file, const std::string func, const int line) : sbuf(Logger::kError, file, func, line), std::iostream(&sbuf) {}
private:
	LoggerStreambuf sbuf;
};

struct LoggerAbort : public std::iostream {
	LoggerAbort() = delete;
	LoggerAbort(const std::string file, const std::string func, const int line) : sbuf(Logger::kAbort, file, func, line), std::iostream(&sbuf) {}
private:
	LoggerStreambuf sbuf;
};

};

#endif

