#include "../include/logger-prime.hpp"

namespace logprime
{
	void Logger::debug(const char* msg)
	{
		if (DEBUG.level <= barier_level)
			log(DEBUG, msg, nullptr, nullptr, NULL);
	}

	void Logger::info(const char* msg)
	{
		if (INFO.level <= barier_level)
			log(INFO, msg, nullptr, nullptr, NULL);
	}
	void Logger::warn(const char* msg)
	{
		if (WARN.level <= barier_level)
			log(WARN, msg, nullptr, nullptr, NULL);
	}
	void Logger::error(const char* msg)
	{
		if (ERROR.level <= barier_level)
			log(ERROR, msg, nullptr, nullptr, NULL);
	}


	void Logger::debug(const char* msg, const char* funcname, const char* filename, const long line)
	{
		if (DEBUG.level <= barier_level)
			log(DEBUG, msg, funcname, filename, line);
	}

	void Logger::info(const char* msg, const char* funcname, const char* filename, const long line)
	{
		if (INFO.level <= barier_level)
			log(INFO, msg, funcname, filename, line);
	}

	void Logger::warn(const char* msg, const char* funcname, const char* filename, const long line)
	{
		if (WARN.level <= barier_level)
			log(WARN, msg, funcname, filename, line);
	}

	void Logger::error(const char* msg, const char* funcname, const char* filename, const long line)
	{
		if (ERROR.level <= barier_level)
			log(ERROR, msg, funcname, filename, line);
	}


	void Logger::setBarierLevel(loglevel level)
	{
		barier_level = level;
	}

	int Logger::setCfgfilePath(std::string path)
	{
		cfg_path.assign(path);

		if (!cfg_path.exists())
		{
			if (std::filesystem::exists(cfg_path.path().parent_path()))
			{
				if (!is_directory(status(cfg_path.path().parent_path())))
					return errors::IS_NOT_DIRECTORY;
			}
			else
				if (!std::filesystem::create_directory(cfg_path.path().parent_path()))
					return errors::DIRECTORY_NOT_CREATED;
		}


		switch (load_cfg())
		{
		case errors::FILE_NOT_OPENED:
		{
			console << fmt::REDBGR << "Cannot open config file.\n" << fmt::DEFAULTTEXT;
			return errors::FILE_NOT_OPENED;
		}
		case errors::CORRUPTED_CFG_FILE:
		{
			console << fmt::REDBGR << "Config file is corrupted.\n" << fmt::DEFAULTTEXT;
			return errors::CORRUPTED_CFG_FILE;
		}
		default:
			break;
		}

		return errors::SUCCESS;
	}


	int Logger::setLogDir(std::string path)
	{
		logs_dir.assign(path);
		file_path.assign(logs_dir/default_filename);

		if (std::filesystem::exists(logs_dir))
		{
			if (!is_directory(status(logs_dir)))
				return errors::IS_NOT_DIRECTORY;
		}
		else
			if (!std::filesystem::create_directory(logs_dir))
				return errors::DIRECTORY_NOT_CREATED;

		prepare_file();
		return errors::SUCCESS;
	}

	int Logger::setMaxFileSize(int size_in_bytes)
	{
		MAX_FILE_SIZE = size_in_bytes;
		return errors::SUCCESS;
	}

	int Logger::setMaxFileLines(int lines)
	{
		MAX_FILE_LINES = lines;
		return errors::SUCCESS;
	}

	int Logger::setMaxFileQuantity(int quantity)
	{
		MAX_FILE_QUANTITY = quantity;
		return errors::SUCCESS;
	}


	void Logger::setFlags(std::bitset<8> bitset)
	{
		std::lock_guard<std::mutex> lock_guard(mutex);
		flags |= bitset;
	}

	void Logger::unsetFlags(std::bitset<8> bitset)
	{
		std::lock_guard<std::mutex> lock_guard(mutex);
		auto temp = flags;
		flags ^= bitset;
		flags &= temp;
	}

}

