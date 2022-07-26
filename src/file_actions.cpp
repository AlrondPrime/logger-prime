#include "../include/logger-prime.hpp"

namespace logprime
{
	int Logger::save_cfg()
	{
		cfg_file.open(cfg_path.path().string(), std::fstream::out | std::fstream::trunc);
		if (!cfg_file.is_open())
			return errors::FILE_NOT_OPENED;

		
		// Not most obvious method to figure out if stringstream is not empty
		ss.clear();
		ss.seekg(ss.beg);
		
		if (std::string buf; !(ss >> buf))
		{			
			cfg_file << "current_path=" << file_path.path().string() << '\n';
		}
		else
		{
			ss.seekg(ss.beg);
			std::string line, key, value;
			while (std::getline(ss, line))
			{
				if (line.at(0) == '#')
				{
					cfg_file << line << '\n';
					continue;
				}

				auto pos = line.find('=');
				if (pos == line.npos)
					return errors::CORRUPTED_CFG_FILE;

				key = line.substr(0, pos);
				cfg_file << key << '=';

				if (key == "current_path")
					cfg_file << file_path.path().string() << '\n';
			}
		}

		cfg_file.close();
		return errors::SUCCESS;
	}

	int Logger::load_cfg()
	{
		if (!cfg_path.exists())
		{
			cfg_file.open(cfg_path.path().string(), std::fstream::out);
			cfg_file.close();
		}

		std::pair<std::chrono::system_clock::rep, std::filesystem::path> pair(0, file_path.path().string());
		for (auto& iter : std::filesystem::directory_iterator(logs_dir))
			if (iter.last_write_time().time_since_epoch().count() > pair.first)
				pair.second = iter.path();

		if (std::filesystem::is_empty(cfg_path))
		{			
			file_path.assign(pair.second);
			return errors::SUCCESS;
		}

		cfg_file.open(cfg_path.path().string(), std::fstream::in);
		if (!cfg_file.is_open())
			return errors::FILE_NOT_OPENED;

		ss << cfg_file.rdbuf();
		cfg_file.close();

		std::string line, key, value;

		while (std::getline(ss, line))
		{
			if (line == "" || line.at(0) == '#')
				continue;

			auto pos = line.find('=');
			if (pos == line.npos)
			{
				return errors::CORRUPTED_CFG_FILE;
			}

			key = line.substr(0, pos);
			value = line.substr(pos + 1, line.size());
			if (key == "current_path")
				if (value == "default_path")
					file_path.assign(default_filename);
				else
				{
					file_path.assign(value);
					if (!file_path.exists())
						file_path.assign(pair.second);
				}
		}

		return errors::SUCCESS;
	}

	size_t Logger::get_file_size()
	{
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
		return std::distance(std::filesystem::directory_iterator(logs_dir), std::filesystem::directory_iterator());
	}

	void Logger::create_new_file()
	{
		if (count_file_quantity() > MAX_FILE_QUANTITY)
		{
			remove_excess_files();
		}

		file.close();
		file_path.assign(generate_filename());
		file_lines = 0;
		if (prepare_file() == errors::FILE_NOT_OPENED)
		{
			console << fmt::REDBGR << "Cannot create log file.\n" << fmt::DEFAULTTEXT;
			return;
		}
	}

	void Logger::remove_excess_files()
	{
		std::multimap<std::filesystem::file_time_type, std::filesystem::path> paths;

		for (auto& iter : std::filesystem::directory_iterator(logs_dir))
			paths.insert({ iter.last_write_time(), iter.path() });

		auto first = paths.begin();
		while (count_file_quantity() > MAX_FILE_QUANTITY)
		{
			std::filesystem::remove(first->second);
			++first;
		}
	}

	int Logger::prepare_file()
	{

		if (file.is_open())
		{
			file.ignore();
			file.clear();
			file.close();
		}

		if (!file_path.exists())
		{
			file.open(file_path.path().string(), std::fstream::out);
			file.close();
		}

		file.open(file_path.path().string(), std::fstream::in | std::fstream::out | std::fstream::app);


		if (!file.is_open())
			return errors::FILE_NOT_OPENED;


		if (count_file_lines() > MAX_FILE_LINES)
		{
			create_new_file();
		}

		if (get_file_size() > MAX_FILE_SIZE)
		{
			create_new_file();
		}

		if (count_file_quantity() > MAX_FILE_QUANTITY)
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