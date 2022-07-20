#include<iostream>
#include<fstream>
#include<filesystem>
#include<thread>
#include<ctime>
#include <string>
#include <bitset>
#include <mutex>

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
		enum flagset // degrees of 2
		{
			CONSOLE_OUTPUT = 1,
			FILE_OUTPUT = 2,


			LAST = 128
		};//last bit with decimal value 128 cannot be used while using <<1 in Logger constructor
	}
}

namespace logprime
{
	class Logger
	{
	private:

		struct logtype
		{
			const char* name; /*DEBUG, INFO, WARN, ERROR, etc. with priorities,
			0 has maximum priority, larger values have lower priority*/
			loglevel level;
			const char* format; //specific text format in control characters for console output
		};

		const logtype ERROR{ "ERROR", loglevel::ERROR, fmt::REDTEXT };
		const logtype INFO{ "INFO", loglevel::INFO, fmt::GREENTEXT };
		const logtype WARN{ "WARN", loglevel::WARN, fmt::YELLOWTEXT };
		const logtype DEBUG{ "DEBUG", loglevel::DEBUG, fmt::BLUETEXT };

		std::bitset<8> flags{ 0 };

		std::ostream& console{ std::cout };
		std::fstream file;
		std::fstream cfg_file;

		std::filesystem::path logs_dir{ "./logs" };
		std::filesystem::directory_entry file_path{ logs_dir.string() + "/log.txt" };
		std::filesystem::directory_entry cfg_path{ "./cfg/logger-prime.cfg" };

		loglevel barier_level{ loglevel::DEBUG };

		size_t file_lines{ 0 }; //number of lines in current using file
		int file_postfix{ 0 }; //postfix for marking files
		int MAX_FILE_SIZE{ 20 * 8 * 1024 }; //size in bytes
		int MAX_FILE_LINES{ 2000 }; //size in lines
		int MAX_FILE_QUANTITY{ 20 };

		std::mutex mutex;


	public:
		explicit Logger(int bitset) :
			flags(bitset << 1)//because std::bitset counts from 0, but enum flagset starts from 1
		{
			if (bitset >= flagset::LAST)
			{
				console << fmt::REDBGR << "Incorrect initial value\n" << fmt::DEFAULTTEXT;
				flags = 0;
			}
			else
				if (load_cfg() == errors::FILE_NOT_OPENED)
				{
					console << fmt::REDBGR << "Cannot open config file.\n" << fmt::DEFAULTTEXT;
				}
		}

		~Logger()
		{
			if (save_cfg() == errors::FILE_NOT_OPENED)
			{
				console << fmt::REDBGR << "Cannot open config file to write changes.\n" << fmt::DEFAULTTEXT;
			}

			file.close();
		}

		void debug(const char* msg);
		void info(const char* msg);
		void warn(const char* msg);
		void error(const char* msg);

		int setLogfilePath(std::string path);
		int setCfgfilePath(std::string path);
		int setLogDir(std::string path);

		void setBarierLevel(loglevel level);

	private:
		void log(const logtype& type, const char* msg)
		{
			if (_log(type, msg) == errors::FILE_NOT_OPENED)
				console << fmt::REDBGR << "File is not open. Cannot write to it.\n" << fmt::DEFAULTTEXT;
		}


		int _log(const logtype& type, const char* msg)
		{
			auto strtime = get_date_time("%X");

			if (flags.test(flagset::CONSOLE_OUTPUT))
			{
				std::lock_guard<std::mutex> lock_guard(mutex);
				console << "[ " << strtime << " ] " <<
					"[ " << type.format << type.name << fmt::DEFAULTTEXT << " ] " <<
					msg << '\n';
			}

			if (flags.test(flagset::FILE_OUTPUT))
			{
				std::lock_guard<std::mutex> lock_guard(mutex);
				if (file_lines >= MAX_FILE_LINES)
					create_new_file();

				file << "[ " << strtime << " ] " <<
					"[ " << type.name << " ] " <<
					msg << '\n';

				++file_lines;
			}

			return errors::SUCCESS;
		}



		std::string get_date_time(const char* format)
		{
			std::string strtime;
			strtime.resize(20);
			time_t now = time(0);
			struct tm newtime;
			localtime_s(&newtime, &now);
			strftime(strtime.data(), strtime.size(), format, &newtime);
			strtime.erase(std::find(strtime.begin(), strtime.end(), '\0'), strtime.end());
			return strtime;
		}

		int prepare_file();
		size_t get_file_size();
		size_t count_file_lines();
		size_t count_file_quantity();
		void create_new_file();
		void remove_excess_files();
		void check_actual_file();

		int load_cfg();
		int save_cfg();

		std::string generate_filename();


	};

}