#include "Logger.hpp"

using namespace wlib;
int main(void) {

	LoggerStream hoge();
	std::cout << "Before Redirect of cout" << std::endl;

	std::cerr << "Before Redirect of cerr" << std::endl;

	Logger::setRedirectionCout(Logger::kDebug);
	Logger::setRedirectionCerr(Logger::kError);

	std::cout << "This is cout output" << std::endl;

	std::cerr << "This is cerr output" << std::endl;

	std::wcout << L"これはワイド文字wcout出力だよ" << std::endl;

	std::wcerr << L"これはワイド文字wcerr出力だよ" << std::endl;

	trace << "This is trace output" << std::endl;

	debug << "This is debug output" << std::endl;

	info << "This is info output" << std::endl;

	warn << "This is warn output" << std::endl;

	fatal << "This is fatal output" << std::endl;

	wtrace << L"This is trace output" << std::endl;

	wdebug << L"This is debug output" << std::endl;

	winfo << L"This is info output" << std::endl;

	wwarn << L"This is warn output" << std::endl;

	wfatal << L"This is fatal output" << std::endl;

	trace << SRCINFO << "This is trace output" << std::endl;

	debug << SRCINFO << "This is debug output" << std::endl;

	info << SRCINFO << "This is info output" << std::endl;

	warn << SRCINFO << "This is warn output" << std::endl;

	fatal << SRCINFO << "This is fatal output" << std::endl;


	wtrace << WSRCINFO << L"This is trace output" << std::endl;

	wdebug << WSRCINFO << L"This is debug output" << std::endl;

	winfo << WSRCINFO << L"This is info output" << std::endl;

	wwarn << WSRCINFO << L"This is warn output" << std::endl;
	
	wfatal << WSRCINFO << L"This is fatal output" << std::endl;


}