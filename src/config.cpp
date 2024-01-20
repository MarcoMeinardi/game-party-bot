#include "config.hpp"

config_t load_config()
{
	config_t config;

	std::ifstream config_file(CONFIG_FILENAME);

	if (!config_file.is_open()) {
		std::cerr << "Cannot find config file" << std::endl;
		config_file.close();
		exit(1);
	}

	if (!(config_file >> config.token >> config.channel_id)) {
		std::cerr << "Error reading config file" << std::endl;
		config_file.close();
		exit(1);
	}

	config_file.close();

	return config;
}
