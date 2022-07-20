#include "../include/logger-prime.hpp"

namespace logprime
{
	void Logger::debug(const char* msg)
	{
		if (DEBUG.level <= barier_level)
			log(DEBUG, msg);
	}
	void Logger::info(const char* msg)
	{
		if (INFO.level <= barier_level)
			log(INFO, msg);
	}
	void Logger::warn(const char* msg)
	{
		if (WARN.level <= barier_level)
			log(WARN, msg);
	}
	void Logger::error(const char* msg)
	{
		if (ERROR.level <= barier_level)
			log(ERROR, msg);
	}

	void Logger::setBarierLevel(loglevel level)
	{
		barier_level = level;
	}


	int Logger::setLogfilePath(std::string path)
	{
		file_path.assign(path);
		if (prepare_file() == errors::FILE_NOT_OPENED)
		{
			console << fmt::REDBGR << "Cannot open specified log file.\n" << fmt::DEFAULTTEXT;
			return errors::FILE_NOT_OPENED;
		}
		return errors::SUCCESS;
	}

	int Logger::setCfgfilePath(std::string path)
	{
		cfg_path.assign(path);
		if (load_cfg() == errors::FILE_NOT_OPENED)
		{
			console << fmt::REDBGR << "Cannot open specified config file.\n" << fmt::DEFAULTTEXT;
			return errors::FILE_NOT_OPENED;
		}
		return errors::SUCCESS;
	}


	int Logger::setLogDir(std::string path)
	{
		logs_dir.assign(path);

		if (std::filesystem::exists(logs_dir))
		{
			if (!is_directory(status(logs_dir)))
				return errors::IS_NOT_DIRECTORY;
		}
		else
			if (!std::filesystem::create_directory(logs_dir))
				return errors::DIRECTORY_NOT_CREATED;
		
		return errors::SUCCESS;
	}
}