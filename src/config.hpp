#ifndef _CONFIG_HPP_
#define _CONFIG_HPP_

#include <string>
#include <dpp/dpp.h>

#define CONFIG_FILENAME ".config"

struct config_t {
	std::string token;
	dpp::snowflake channel_id;
};

config_t load_config();

#endif  // _CONFIG_HPP_
