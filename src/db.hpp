#ifndef _DB_HPP_
#define _DB_HPP_

#include <dpp/dpp.h>

#define DB_FILENAME "data/db.dat"

extern std::vector<dpp::snowflake> playing_today;
extern std::vector<dpp::snowflake> playing_tomorrow;
extern std::mutex playing_today_mutex;
extern std::mutex playing_tomorrow_mutex;

extern dpp::snowflake info_message_id;

void load_db();
void update_db();

#endif  // _DB_HPP_
