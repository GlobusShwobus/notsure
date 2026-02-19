#pragma once

#include <string>

#include <mysql_driver.h>              
#include <cppconn/prepared_statement.h>   

#include <memory>
#include <unordered_map>
#include "BoolMessage.h"
#include "Insert_statement.h"

//TODO:: call procedure?

namespace badSQL
{
	template <typename T>
	concept BINDABLE = requires(sql::PreparedStatement * stmt, const T & v)
	{
		user_bind(stmt, v);
	};

	template <typename T>
	concept BULK_BINDABLE = requires(sql::PreparedStatement * stmt, const T & v, std::size_t & i)
	{
		user_bind_bulk(stmt, v, i);
	};

	class SQLInserter final
	{
	public:
		SQLInserter() = default;

		void terminate() noexcept;

		bool is_connected()noexcept;
		
		badCore::BoolMessage connect(const std::string& user, const std::string& service) noexcept;

		template <BINDABLE T>
		badCore::BoolMessage inject(const T& item, const FinalizedStatement& statement)
		{
			if (!is_connected()) {
				terminate();
				return badCore::BoolMessage::failure("No Connection");
			}

			try {
				sql::PreparedStatement* pstmt = get_pstmt(statement.statement);

				user_bind(pstmt, item);

				pstmt->execute();
			}
			catch (const sql::SQLException& e) {
				terminate();
				return badCore::BoolMessage::failure(std::string("Connection terminated: ") + e.what());
			}
			catch (...) {
				terminate();
				return badCore::BoolMessage::failure("Connection terminated: Unknown error");
			}

			return badCore::BoolMessage::success();
		}

		template <std::input_iterator InputIt>
		badCore::BoolMessage inject_bulk(InputIt begin, InputIt end, const FinalizedStatement& statement)
			requires BULK_BINDABLE<std::iter_reference_t<InputIt>>
		{
			if (!is_connected()) {
				terminate();
				return badCore::BoolMessage::failure("No Connection");
			}

			if (begin == end)
				return badCore::BoolMessage::failure("Nothing to insert");

			try {
				sql::PreparedStatement* pstmt = get_pstmt(statement.statement);

				std::size_t index = 1;
				for (; begin != end; ++begin) {
					user_bind_bulk(pstmt, *begin, index);
				}

				pstmt->execute();
			}
			catch (const sql::SQLException& e) {
				terminate();
				return badCore::BoolMessage::failure(std::string("Connection terminated: ") + e.what());
			}
			catch (...) {
				terminate();
				return badCore::BoolMessage::failure("Connection terminated: Unknown error");
			}

			return badCore::BoolMessage::success();
		}

		badCore::BoolMessage set_auto_commit(bool state)noexcept;

		badCore::BoolMessage commit() noexcept;
	private:

		sql::PreparedStatement* get_pstmt(const std::string& sql);

	private:
		//ORDER MATTERS
		std::unique_ptr<sql::Connection> mConnect = nullptr;
		std::unordered_map<
			std::string,//key
			std::unique_ptr<sql::PreparedStatement>,//type
			std::hash<std::string_view>,//hasing with a view obj instead of a string 
			std::equal_to<>// string == const char*
		> cache;
	};
}
