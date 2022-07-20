#include "../include/logger-prime.hpp"
using namespace logprime;
using namespace std;

void thread_log(Logger& logger, loglevel level, const char* msg)
{
	switch (level)
	{
	case loglevel::DEBUG:
		logger.debug(msg);
		break;

	case loglevel::INFO:
		logger.info(msg);
		break;

	case loglevel::WARN:
		logger.warn(msg);
		break;

	case loglevel::ERROR:
		logger.error(msg);
		break;
	}
}


int main()
{
	logprime::Logger mylogger(flagset::CONSOLE_OUTPUT | flagset::FILE_OUTPUT);
	mylogger.setCfgfilePath("./cfg/logger-prime.cfg");
	mylogger.setLogDir("./logs");
	mylogger.setLogfilePath("./logs/log.txt");
	
	vector<thread> vc;

	for (size_t i{}; i < 250; ++i)
	{
		vc.push_back(thread(thread_log, std::ref(mylogger), loglevel::DEBUG, "debug"));
		vc.push_back(thread(thread_log, std::ref(mylogger), loglevel::INFO, "info"));
		vc.push_back(thread(thread_log, std::ref(mylogger), loglevel::WARN, "warn"));
		vc.push_back(thread(thread_log, std::ref(mylogger), loglevel::ERROR, "error"));
	}

	for (auto& thr : vc)
		thr.detach();

	this_thread::sleep_for(chrono::seconds(1));
	return 0;
}