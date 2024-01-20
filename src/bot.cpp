#include <iostream>
#include <string>
#include <vector>
#include <dpp/dpp.h>

#include "db.hpp"
#include "config.hpp"
#include "utils.hpp"

config_t config;

std::vector<dpp::snowflake> playing_today;
std::mutex playing_today_mutex;
std::vector<dpp::snowflake> playing_tomorrow;
std::mutex playing_tomorrow_mutex;
dpp::message info_message;

void update_info(dpp::cluster& bot)
{
	std::ostringstream oss;
	if (playing_today.empty()) {
		 oss << "No one is gaming tonight :sob:" << std::endl;
	} else {
		if (playing_today.size() == 1) {
			oss << "1 person is gaming tonight, don't leave him (scusa Sofia) alone!" << std::endl;
		} else {
			oss << playing_today.size() << " people are gaming tonight! Join the party :partying_face:" << std::endl;
		}
		playing_today_mutex.lock();
		for (dpp::snowflake id : playing_today) {
			oss << "<@" + std::to_string(id) + ">" << std::endl;
		}
		playing_today_mutex.unlock();
	}

	if (!playing_tomorrow.empty()) {
		oss << std::endl;
		if (playing_tomorrow.size() == 1) {
			oss << "1 person will be gaming tomorrow" << std::endl;
		} else {
			oss << playing_tomorrow.size() << " people will be gaming tomorrow" << std::endl;
		}
		playing_tomorrow_mutex.lock();
		for (dpp::snowflake id : playing_tomorrow) {
			oss << "<@" + std::to_string(id) + ">" << std::endl;
		}
		playing_tomorrow_mutex.unlock();
	}

	info_message.set_content(oss.str());
	bot.message_edit(info_message);
}

void send_messages(dpp::cluster& bot)
{
	dpp::message buttons_message = dpp::message(config.channel_id, "")
		.add_component(dpp::component()
			.set_type(dpp::cot_action_row)
			.add_component(
				dpp::component()
					.set_type(dpp::cot_button)
					.set_label("Play tonight")
					.set_style(dpp::cos_success)
					.set_id("play0")
			)
			.add_component(
				dpp::component()
					.set_type(dpp::cot_button)
					.set_label("Leave tonight")
					.set_style(dpp::cos_danger)
					.set_id("leave0")
			)
		)
		.add_component(dpp::component()
			.set_type(dpp::cot_action_row)
			.add_component(
				dpp::component()
					.set_type(dpp::cot_button)
					.set_label("Play tomorrow")
					.set_style(dpp::cos_success)
					.set_id("play1")
			)
			.add_component(
				dpp::component()
					.set_type(dpp::cot_button)
					.set_label("Leave tomorrow")
					.set_style(dpp::cos_danger)
					.set_id("leave1")
			)
		);

	bot.message_create(buttons_message);

	info_message = bot.message_create_sync(
		dpp::message(config.channel_id, "Loading gamers...")
	);
	update_info(bot);
}

int main ()
{
	config = load_config();
	load_db();
	dpp::cluster bot(config.token);

	bot.on_ready([&bot] (__attribute__((unused)) const dpp::ready_t& event)
	{
		send_messages(bot);
	});

	bot.on_button_click([&bot] (const dpp::button_click_t& event)
	{
		dpp::snowflake user_id = event.command.usr.id;
		std::string user_name = event.command.member.get_nickname();

		std::string button_id = event.custom_id;
		std::vector<dpp::snowflake>& playing = \
			button_id.back() == '0' ? playing_today : playing_tomorrow;
		std::mutex& playing_mutex = \
			button_id.back() == '0' ? playing_today_mutex : playing_tomorrow_mutex;
		std::vector<dpp::snowflake>::iterator user_it = find_user(user_id, playing_mutex, playing);

		std::string response;
		bool need_update = false;

		if (event.custom_id == "play0") {
			std::cerr << "@" << user_name << " joined tonight" << std::endl;
			playing_mutex.lock();
			if (user_it == playing.end()) {
				playing.push_back(user_id);
				response = "Let's go gamer!";
				need_update = true;
			} else {
				response = "I know you really want to game, but please, calm down";
			}
			playing_mutex.unlock();
		} else if (event.custom_id == "leave0") {
			std::cerr << "@" << user_name << " left tonight" << std::endl;
			playing_mutex.lock();
			if (user_it != playing.end()) {
				playing.erase(user_it);
				response = "That's sad :cry:";
				need_update = true;
			} else {
				response = "You are already being a bad gamer :angry:";
			}
			playing_mutex.unlock();
		} else if (event.custom_id == "play1") {
			std::cerr << "@" << user_name << " joined tomorrow" << std::endl;
			playing_mutex.lock();
			if (user_it == playing.end()) {
				playing.push_back(user_id);
				response = "See you tomorrow!";
				need_update = true;
			} else {
				response = "Tomorrow will come, be patient";
			}
			playing_mutex.unlock();
		} else if (event.custom_id == "leave1") {
			std::cerr << "@" << user_name << " left tomorrow" << std::endl;
			playing_mutex.lock();
			if (user_it != playing.end()) {
				playing.erase(user_it);
				response = "At least you're playing today, right?";
				need_update = true;
			} else {
				response = "You have never told me you will be playing";
			}
			playing_mutex.unlock();
		} else {
			response = "How did you get here?";
		}

		event.reply(
			dpp::message(response).set_flags(dpp::m_ephemeral)
		);

		if (need_update) {
			update_info(bot);
			update_db();
		}
	});

	bot.start(dpp::st_wait);
	return 0;
}
