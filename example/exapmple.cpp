#include "../include/logger-prime.hpp"

using namespace logprime;
using namespace std;

int main()
{
	logprime::Logger mylogger(FILE_OUTPUT | CONSOLE_OUTPUT);
	mylogger.debug("Some debug message");
	mylogger.info("Some info message");
	mylogger.warn("Some warn messageg");
	mylogger.error("Some error message");

	

	return 0;
}