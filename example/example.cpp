// Macro to enable trace information about function name, etc.
#define TRACE

#include "../include/logger-prime.hpp"
using namespace logprime;
using namespace std;

// Function to test multithread calling
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
	// Constructing our logger object with console printing only
	Logger mylogger(flagset::CONSOLE_OUTPUT);


	// Setting some default settings, which can be modified later via API
	mylogger.setCfgfilePath("./cfg/logger-prime.cfg");
	mylogger.setLogDir("./logs");
	mylogger.setMaxFileLines(2000);
	mylogger.setMaxFileQuantity(20);


	// Testing deleting excess files and testing the file lines limit
	mylogger.setFlags(flagset::FILE_OUTPUT);
	mylogger.unsetFlags(flagset::CONSOLE_OUTPUT);
	mylogger.setMaxFileQuantity(2);
	mylogger.setMaxFileLines(4);
	for (size_t i{}; i < 16; ++i)
		mylogger.debug((std::string("Chunk #") + to_string(i)).c_str());
	/* We created several files, but deleted all but two
	And you can see chunks 8 - 11 in first file, chunks 12 - 15 in second file,
	other files with other chunks have been removed */

	// Restoring recommended values
	mylogger.setMaxFileLines(2000);
	mylogger.setMaxFileQuantity(20);


	// Testing control flags
	mylogger.setFlags(flagset::CONSOLE_OUTPUT);
	mylogger.unsetFlags(flagset::FILE_OUTPUT);
	mylogger.debug("This message will be shown only in console");

	mylogger.unsetFlags(flagset::CONSOLE_OUTPUT);
	mylogger.info("This message will not be shown");

	mylogger.setFlags(flagset::CONSOLE_OUTPUT | flagset::DETAILED_OUTPUT | flagset::FILE_OUTPUT);
	mylogger.warn("This message will contain function name, file name and line where it was written, it will be shown at console and file");

	mylogger.unsetFlags(flagset::CONSOLE_OUTPUT | flagset::DETAILED_OUTPUT);
	mylogger.setFlags(flagset::FILE_OUTPUT);
	mylogger.error("This message wil be wtitten to file only");


	// Testing barier level
	mylogger.setFlags(flagset::CONSOLE_OUTPUT);
	mylogger.setBarierLevel(loglevel::WARN);
	mylogger.info("This message will not be written");
	mylogger.warn("This message will be written at console and file");
	mylogger.error("This message will be written at console and file");

	// Setting default barier level
	mylogger.setBarierLevel(loglevel::DEBUG);


	// Testing logging in multithread environment via ulimate caller function 'thread_log'
	mylogger.setFlags(flagset::CONSOLE_OUTPUT | flagset::FILE_OUTPUT);
	mylogger.setMaxFileQuantity(20);
	mylogger.setMaxFileLines(500);

	vector<thread> vector_thread;
	for (size_t i{}; i < 250; ++i)
	{
		vector_thread.push_back(thread(thread_log, std::ref(mylogger), loglevel::INFO, "multisink info message"));
		vector_thread.push_back(thread(thread_log, std::ref(mylogger), loglevel::DEBUG, "multisink debug message"));
		vector_thread.push_back(thread(thread_log, std::ref(mylogger), loglevel::WARN, "multisink warn message"));
		vector_thread.push_back(thread(thread_log, std::ref(mylogger), loglevel::ERROR, "multisink error message"));
	}
	
	for (auto& thr : vector_thread)
		thr.join();

	this_thread::sleep_for(chrono::seconds(1));

	return 0;
}


