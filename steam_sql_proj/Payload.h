#pragma once

#include <optional>
#include <string> 
#include <cppconn/prepared_statement.h>
#include "json.hpp"

namespace badSQL
{
	//use optional to trigger failures for sql
	struct Payload {
		std::optional<std::size_t> recipient_id;
		std::optional<std::string> label;
		std::optional<nlohmann::json> data;//data = {} does not initalize the json, it calls the std::optional constructor... a pitfall to remember
	};


	void user_bind(sql::PreparedStatement* pstmt, const Payload& item);

	void user_bind_bulk(sql::PreparedStatement* pstmt, const Payload& item, std::size_t& index);
}