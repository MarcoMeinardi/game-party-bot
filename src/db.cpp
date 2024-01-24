#include "db.hpp"

static void read_vector(std::ifstream& file, std::vector<dpp::snowflake>& vec)
{
	size_t size;
	if (!file.read(reinterpret_cast<char*>(&size), sizeof(size_t))) {
		std::cerr << "Error reading vector size from db" << std::endl;
		file.close();
		exit(1);
	}

	vec.resize(size);
	if (!file.read(reinterpret_cast<char*>(vec.data()), size * sizeof(dpp::snowflake))) {
		std::cerr << "Error reading vector from db" << std::endl;
		file.close();
		exit(1);
	}
}

static void write_vector(std::ofstream& file, std::vector<dpp::snowflake>& vec)
{
	size_t size = vec.size();
	if (!file.write(reinterpret_cast<char*>(&size), sizeof(size_t))) {
		std::cerr << "Error writing vector size to db" << std::endl;
		file.close();
		exit(1);
	}

	if (!file.write(reinterpret_cast<char*>(vec.data()), size * sizeof(dpp::snowflake))) {
		std::cerr << "Error writing vector to db" << std::endl;
		file.close();
		exit(1);
	}
}

static void read_snowflake(std::ifstream& file, dpp::snowflake& id)
{
	if (!file.read(reinterpret_cast<char*>(&id), sizeof(dpp::snowflake))) {
		std::cerr << "Error reading snowflake from db" << std::endl;
		file.close();
		exit(1);
	}
}

static void write_snowflake(std::ofstream& file, dpp::snowflake& id)
{
	if (!file.write(reinterpret_cast<char*>(&id), sizeof(dpp::snowflake))) {
		std::cerr << "Error writing snowflake to db" << std::endl;
		file.close();
		exit(1);
	}
}

static void create_db()
{
	std::ofstream db_file(DB_FILENAME, std::ios::binary);

	if (!db_file.is_open()) {
		std::cerr << "Cannot create db" << std::endl;
		db_file.close();
		exit(1);
	}

	size_t empty_db[2] = {0, 0};
	if (!db_file.write(reinterpret_cast<char*>(empty_db), sizeof(empty_db))) {
		std::cerr << "Error writing empty db" << std::endl;
		db_file.close();
		exit(1);
	}

	db_file.close();
}

void load_db()
{
	std::ifstream db_file(DB_FILENAME, std::ios::binary);

	if (!db_file.is_open()) {
		std::cerr << "Cannot find db, creating it" << std::endl;
		db_file.close();
		create_db();
	} else {
		read_vector(db_file, playing_today);
		read_vector(db_file, playing_tomorrow);
		read_snowflake(db_file, info_message_id);
		db_file.close();
	}
}

void update_db()
{
	static std::mutex db_mutex;

	std::vector<dpp::snowflake> playing_today_copy;
	playing_today_mutex.lock();
	playing_today_copy = playing_today;
	playing_today_mutex.unlock();

	std::vector<dpp::snowflake> playing_tomorrow_copy;
	playing_tomorrow_mutex.lock();
	playing_tomorrow_copy = playing_tomorrow;
	playing_tomorrow_mutex.unlock();

	db_mutex.lock();
	std::ofstream db_file(DB_FILENAME, std::ios::binary);

	if (!db_file.is_open()) {
		std::cerr << "Cannot modify or create db" << std::endl;
		db_file.close();
		exit(1);
	}

	write_vector(db_file, playing_today_copy);
	write_vector(db_file, playing_tomorrow_copy);
	write_snowflake(db_file, info_message_id);

	db_file.close();
	db_mutex.unlock();
}
