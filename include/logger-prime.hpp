#include<iostream>
#include<fstream>
#include<filesystem>
#include<thread>
#include<ctime>


#include "format.hpp"

namespace logprime
{
	class Logger
	{
	private:
		enum errors
		{
			FILE_NOT_OPENED = 1,

		};

		struct logtype
		{
			const char* name;
			const char* color;
		};

		const logtype DEBUG{ "DEBUG", fmt::BLUETEXT };
		const logtype INFO{ "INFO", fmt::GREENTEXT };
		const logtype WARN{ "WARN", fmt::YELLOWTEXT };
		const logtype ERROR{ "ERROR", fmt::REDTEXT };
		//const logtype INTERNAL_ERROR{ "INTERNAL_ERROR", fmt::REDTEXT };

		std::bitset<8> flags{ 0 };

		std::ostream& console{ std::cout };
		std::filesystem::path dir{ "logs" };
		std::ofstream file;

	public:
		explicit Logger(int flagset) :
			flags(flagset << 1)
		{

		}

		void debug(const char* msg)
		{
			log(DEBUG, msg);

		}
		void info(const char* msg)
		{
			log(INFO, msg);
		}
		void warn(const char* msg)
		{
			log(WARN, msg);
		}
		void error(const char* msg)
		{
			log(ERROR, msg);
		}



	private:
		int _log(const logtype& type, const char* msg)
		{
			time_t now = time(0);
			struct tm newtime;
			localtime_s(&newtime, &now);
			char buf[10];
			strftime(buf, sizeof(buf), "%X", &newtime);

			if (flags.test(CONSOLE_OUTPUT))
			{
				console << "[ " << buf << " ] ";
				console << "[ " << type.color << type.name << fmt::DEFAULTTEXT << " ] ";
				console << msg;
				console << '\n';
			}

			if (flags.test(FILE_OUTPUT))
			{
				if (!check_file())
					return FILE_NOT_OPENED;


				file << "[ " << buf << " ] ";
				file << "[ " << type.name << " ] ";
				file << msg;
				file << '\n';
			}

			return 0;
		}

		void log(const logtype& type, const char* msg)
		{
			int result = _log(type, msg);
			if (result == FILE_NOT_OPENED)
				console << fmt::REDBGR << "File is not open. Cannot write to it.\n" << fmt::DEFAULTTEXT;
		}


		bool check_file()
		{
			if (std::filesystem::exists(dir))
			{
				if(!is_directory(status(dir)))
					return false;
			}
			else
				if (!std::filesystem::create_directory(dir))
					return false;

			file.open("./logs/log.txt", std::ios::app);
			if (!file.is_open())
				return false;
			else
				return true;
		}
	};
}