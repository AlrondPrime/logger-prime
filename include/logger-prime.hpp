#include<iostream>
#include<fstream>
#include<filesystem>
#include<thread>
#include<ctime>
#include <string>
#include <sstream>
#include <bitset>
#include <mutex>
#include <map>


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
			CORRUPTED_CFG_FILE,

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
			DETAILED_OUTPUT = 4,

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
		const logtype WARN{ "WARNING", loglevel::WARN, fmt::YELLOWTEXT };
		const logtype DEBUG{ "DEBUG", loglevel::DEBUG, fmt::BLUETEXT };

		std::bitset<8> flags{ 0 };

		std::ostream& console{ std::cout };
		std::fstream file;
		std::fstream cfg_file;

		std::stringstream ss;
		std::filesystem::path logs_dir{ "./" };
		const char* default_filename{ "log.txt" };
		const char* default_cfg_path{ "./logger-prime.cfg" };
		std::filesystem::directory_entry file_path{ "" };
		std::filesystem::directory_entry cfg_path{ "" };

		loglevel barier_level{ loglevel::DEBUG };

		int file_postfix{ 0 }; //postfix for marking files
		size_t file_lines{ 0 }; //number of lines in current using file
		size_t MAX_FILE_SIZE{ 100 * 1024 }; //size in bytes (100kb)
		size_t MAX_FILE_LINES{ 2000 }; //size in lines
		size_t MAX_FILE_QUANTITY{ 20 };

		std::mutex mutex;

	public:
		explicit Logger(std::bitset<8> bitset) :
			flags(bitset)
		{
			if (bitset.test(7))
			{
				console << fmt::REDBGR << "Incorrect initial value\n" << fmt::DEFAULTTEXT;
				flags = 0;
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


		// api section

		void debug(const char* msg);
		void info(const char* msg);
		void warn(const char* msg);
		void error(const char* msg);

		void debug(const char* msg, const char* funcname, const char* filename, const long line);
		void info(const char* msg, const char* funcname, const char* filename, const long line);
		void warn(const char* msg, const char* funcname, const char* filename, const long line);
		void error(const char* msg, const char* funcname, const char* filename, const long line);

		int setCfgfilePath(std::string path);
		int setLogDir(std::string path);
		int setMaxFileSize(int size_in_bytes);
		int setMaxFileLines(int lines);
		int setMaxFileQuantity(int quantity);

		void setBarierLevel(loglevel level);
		void setFlags(std::bitset<8> bitset);
		void unsetFlags(std::bitset<8> bitset);

	private:
		void log(const logtype& type, const char* msg, const char* funcname, const char* filename, const long line)
		{
			if (_log(type, msg, funcname, filename, line) == errors::FILE_NOT_OPENED)
			{
				console << fmt::REDBGR << "File is not open. Cannot write to it.\n" << fmt::DEFAULTTEXT;
			}
		}
		int _log(const logtype& type, const char* msg, const char* funcname, const char* filename, const long line)
		{
			auto strtime = get_date_time("%d-%m-%Y %X");

			if (flags.test(degree_of_two(flagset::CONSOLE_OUTPUT)))
			{
				std::lock_guard<std::mutex> lock_guard(mutex);
				console << "[ " << strtime << " ] " <<
					"[ " << type.format << type.name << fmt::DEFAULTTEXT << " ] " <<
					msg;


				if (flags.test(degree_of_two(flagset::DETAILED_OUTPUT)) && funcname != nullptr)
					console << fmt::GREENTEXT << "   from function '" << funcname
					<< "' in file " << filename << ':' << line << fmt::DEFAULTTEXT;

				console << '\n';
			}

			if (flags.test(degree_of_two(flagset::FILE_OUTPUT)))
			{
				std::lock_guard<std::mutex> lock_guard(mutex);
				if (file_lines >= MAX_FILE_LINES || get_file_size() >= MAX_FILE_SIZE)
					create_new_file();

				file << "[ " << strtime << " ] " <<
					"[ " << type.name << " ] " <<
					msg;

				if (flags.test(degree_of_two(flagset::DETAILED_OUTPUT)) && funcname != nullptr)
					file << "   from function '" << funcname
					<< "' in file " << filename << ':' << line;
				file << '\n';

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

		int degree_of_two(int value)
		{
			for (int i{ 0 }; i <= 7; ++i)
				if (value & static_cast<int>(std::pow(2, i)))
					return i;
		}

		//file_actions section

		int prepare_file();
		size_t get_file_size();
		size_t count_file_lines();
		size_t count_file_quantity();
		void create_new_file();
		void remove_excess_files();

		int load_cfg();
		int save_cfg();

		std::string generate_filename();


	};

}

#ifdef TRACE
#define info(msg) info(msg, __func__, __FILE__, __LINE__)
#define debug(msg) debug(msg, __func__, __FILE__, __LINE__)
#define warn(msg) warn(msg, __func__, __FILE__, __LINE__)
#define error(msg) error(msg, __func__, __FILE__, __LINE__)
#endif