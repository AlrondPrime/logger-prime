#include <bitset>

namespace logprime
{
	namespace fmt
	{
		const char BLUETEXT[]{ "\033[36;1m" };
		const char GREENTEXT[]{ "\033[32;1m" };
		const char YELLOWTEXT[]{ "\033[33;1m" };
		const char REDTEXT[]{ "\033[31;1m" };
		const char REDBGR[]{ "\033[41;1m" };
		const char DEFAULTTEXT[]{ "\033[40;37;1m" };
	}

	enum flagset
	{
		CONSOLE_OUTPUT = 1,
		FILE_OUTPUT = 2,

	};//last bit with decimal value 128 cannot be used while using <<1 in Logger constructor
}