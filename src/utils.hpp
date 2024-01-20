#ifndef _UTILS_HPP_
#define _UTILS_HPP_

#include <vector>
#include <dpp/dpp.h>

std::vector<dpp::snowflake>::iterator find_user(
	dpp::snowflake,
	std::mutex&,
	std::vector<dpp::snowflake>&
);

#endif  // _UTILS_HPP_
