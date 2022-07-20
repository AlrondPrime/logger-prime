#include <bitset>

namespace logprime
{
	enum class loglevel
	{
		ERROR,
		WARN,
		INFO,
		DEBUG,
	};

	namespace errors
	{
		enum errors
		{
			SUCCESS,
			FILE_NOT_EXISTS,
			FILE_NOT_OPENED,
			DIRECTORY_NOT_CREATED,
			IS_NOT_DIRECTORY,

		};
	}

	namespace fmt
	{
		const char BLUETEXT[]{ "\033[36;1m" };
		const char GREENTEXT[]{ "\033[32;1m" };
		const char YELLOWTEXT[]{ "\033[33;1m" };
		const char REDTEXT[]{ "\033[31;1m" };
		const char REDBGR[]{ "\033[41;1m" };
		const char DEFAULTTEXT[]{ "\033[40;37;1m" };
	}

	namespace flagset
	{
		enum flagset
		{
			CONSOLE_OUTPUT = 1,
			FILE_OUTPUT = 2,


			LAST = 128
		};//last bit with decimal value 128 cannot be used while using <<1 in Logger constructor
	}
}