#include "../include/logger-prime.hpp"

namespace logprime
{
	int Logger::save_cfg()
	{
		cfg_file.open(cfg_path.path().string(), std::fstream::out | std::ios::trunc);
		if (!cfg_file.is_open())
			return errors::FILE_NOT_OPENED;
		cfg_file << file_path.path().string();
		cfg_file.close();
		return errors::SUCCESS;
	}

	int Logger::load_cfg()
	{
		cfg_file.open(cfg_path.path().string(), std::fstream::in);
		if (!cfg_file.is_open())
			return errors::FILE_NOT_OPENED;
		std::string str;
		std::getline(cfg_file, str);
		file_path.assign(str);
		cfg_file.close();
		return errors::SUCCESS;
	}

	size_t Logger::get_file_size()
	{
		//TODO test get_file_size()
		return std::filesystem::file_size(file_path);		
	}

	size_t Logger::count_file_lines()
	{
		auto state_backup = file.rdstate();
		file.clear();
		auto pos_backup = file.tellg();
		file.seekg(0);
		file_lines = std::count(std::istreambuf_iterator<char>(file),
			std::istreambuf_iterator<char>(), '\n');
		file.unget();
		file.clear();
		file.seekg(pos_backup);
		file.setstate(state_backup);
		return file_lines;
	}

	size_t Logger::count_file_quantity()
	{
		//TODO test count_file_quantity
		return std::distance(std::filesystem::directory_iterator(logs_dir), std::filesystem::directory_iterator());
	}

	void Logger::create_new_file()
	{
		file.close();
		file_path.assign(generate_filename());
		file_lines = 0;
		if (prepare_file() == errors::FILE_NOT_OPENED)
		{
			console << fmt::REDBGR << "Cannot find last log file which is expected.\n" << fmt::DEFAULTTEXT;
			return;
		}
	}

	void Logger::remove_excess_files()
	{
		//TODO remove_excess_files()
	}

	void Logger::check_actual_file()
	{
		//TODO check_actual_file()
		for (const auto& entry : std::filesystem::directory_iterator(logs_dir.c_str()))
			std::cout << entry.path() << std::endl;
	}

	int Logger::prepare_file()
	{
		

		if (!file_path.exists())
		{
			file.open(file_path.path().string(), std::fstream::out);
			file.close();
		}


		file.open(file_path.path().string(), std::fstream::in | std::fstream::out | std::fstream::app);
		if (!file.is_open())
			return errors::FILE_NOT_OPENED;

		if (count_file_lines() >= MAX_FILE_LINES)
		{
			create_new_file();
		}

		if (get_file_size() >= MAX_FILE_SIZE)
		{
			create_new_file();
		}

		if (count_file_quantity() >= MAX_FILE_QUANTITY)
		{
			remove_excess_files();
		}

		return errors::SUCCESS;
	}

	std::string Logger::generate_filename()
	{
		std::string filename;
		filename.append(logs_dir.string() + "/log_");
		filename.append(get_date_time("%d-%m-%Y"));
		filename.append("_" + std::to_string(file_postfix));
		filename.append(".txt");
		++file_postfix;
		return filename;
	}
}