#include "utils.hpp"

std::vector<dpp::snowflake>::iterator find_user(dpp::snowflake id,
			std::mutex& mutex, std::vector<dpp::snowflake>& vec)
{
	std::lock_guard<std::mutex> lock(mutex);
	return std::find_if(
		vec.begin(),
		vec.end(),
		[id] (const dpp::snowflake& p) { return p == id; }
	);
}
