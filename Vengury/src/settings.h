#pragma once 

#include "common.hpp"

namespace Vengury
{
	class Settings
	{
	public:
		explicit Settings() = default;
		~Settings() = default;

		nlohmann::json options;
		nlohmann::json default_options =
		R"({
			"overlay_enabled": true
		})"_json;

		bool Save() const
		{
			std::string settings_file = std::getenv("appdata");
			settings_file += "\\Vengury\\settings.json";

			std::ofstream file(settings_file, std::ios::out | std::ios::trunc);
			file << options.dump(4);
			file.close();
			return true;
		}

		bool Load()
		{
			std::string settings_file = std::getenv("appdata");
			settings_file += "\\Vengury\\settings.json";

			std::ifstream file(settings_file);

			if (!file.is_open())
			{
				WriteDefaultConfig();
				file.open(settings_file);
			}

			file >> options;

			bool should_save = false;
			for (auto& e : default_options.items())
			{
				if (options.count(e.key()) == 0)
				{
					should_save = true;
					options[e.key()] = e.value();
				}
					
			}

			if (should_save)
			{
				LOG(INFO) << "Updating settings.";
				Save();
			}

			return true;
		}

	private:
		bool WriteDefaultConfig()
		{
			std::string settings_file = std::getenv("appdata");
			settings_file += "\\Vengury\\settings.json";

			std::ofstream file(settings_file, std::ios::out);
			file << default_options.dump(4);
			file.close();

			options.clear();
			options = default_options;

			return true;
		}
	};

	inline Settings g_settings;
}
