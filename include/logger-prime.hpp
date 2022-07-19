#include<iostream>
#include<fstream>
#include<filesystem>
#include<thread>
#include<ctime>
#include <string>

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
			const char* level; //DEBUG, INFO, WARN, ERROR, etc.
			const char* format; //specific text format in control characters for console output
			const int priority; // 0 has maximum priority, larger values have lower priority 
		};



		const logtype ERROR{ "ERROR", fmt::REDTEXT, 0 };
		const logtype WARN{ "WARN", fmt::YELLOWTEXT, 1 };
		const logtype INFO{ "INFO", fmt::GREENTEXT, 2 };
		const logtype DEBUG{ "DEBUG", fmt::BLUETEXT, 3 };

		std::bitset<8> flags{ 0 };

		std::ostream& console{ std::cout };
		std::fstream file;
		std::fstream cfg_file;

		std::filesystem::path logs_dir{ "logs" };
		std::filesystem::directory_entry file_path{ logs_dir.string() + "/default_log.txt"};
		

		int file_lines{ 0 }; //number of lines in current using file
		int file_postfix{ 0 }; //postfix for marking files
		int MAX_FILE_SIZE{ 20*8*1024 }; //size in bytes
		int MAX_FILE_LINES{ 8 }; //size in lines
		int MAX_FILE_QUANTITY{ 20 };

	public:
		explicit Logger(int bitset) :
			flags(bitset << 1)//because std::bitset counts from 0, but enum flagset starts from 1
		{
			if (flags.test(FILE_OUTPUT))
			{				
				if (load_cfg() == FILE_NOT_OPENED)
				{
					console << fmt::REDBGR << "Cannot open config file.\n" << fmt::DEFAULTTEXT;
					console << fmt::REDBGR << "Failed to construct Logger object.\n" << fmt::DEFAULTTEXT;
					return;
				}
				
				if (prepare_file() == FILE_NOT_OPENED)
				{
					console << fmt::REDBGR << "Cannot find last log file which is expected.\n" << fmt::DEFAULTTEXT;
					console << fmt::REDBGR << "Failed to construct Logger object.\n" << fmt::DEFAULTTEXT;
					return;
				}

			}
		}

		~Logger()
		{

			if (save_cfg() == FILE_NOT_OPENED)
			{
				console << fmt::REDBGR << "Cannot open config file.\n" << fmt::DEFAULTTEXT;
				console << fmt::REDBGR << "Failed to construct Logger object.\n" << fmt::DEFAULTTEXT;
			}

			file.close();
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
			auto strtime = get_date_time("%X");

			if (flags.test(CONSOLE_OUTPUT))
			{				
				console << "[ " << strtime << " ] ";
				console << "[ " << type.format << type.level << fmt::DEFAULTTEXT << " ] ";
				console << msg;
				console << '\n';
			}

			if (flags.test(FILE_OUTPUT))
			{
				if (file_lines >= MAX_FILE_LINES)
					create_new_file();

				file << "[ " << strtime << " ] ";
				file << "[ " << type.level << " ] ";
				file << msg;
				file << '\n';
				++file_lines;
			}

			return 0;
		}

		void log(const logtype& type, const char* msg)
		{
			if (_log(type, msg) == FILE_NOT_OPENED)
				console << fmt::REDBGR << "File is not open. Cannot write to it.\n" << fmt::DEFAULTTEXT;
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
		int get_file_size();
		int count_file_lines();
		int count_file_quantity();
		void create_new_file();
		void remove_excess_files();
		void check_actual_file();

		int load_cfg();
		int save_cfg();

		std::string generate_filename();


	};
}