#include "SQL_inserter.h"
#include "CharBuffer.h"

namespace badSQL
{
	void SQLInserter::terminate() noexcept
	{
		cache.clear();
		mConnect.reset();
	}

	bool SQLInserter::is_connected()noexcept
	{
		if (!mConnect) {
			return false;
		}

		try {
			return mConnect->isValid();
		}
		catch (...) {
			return false;
		}
	}
	badCore::BoolMessage SQLInserter::connect(const std::string& user, const std::string& service) noexcept
	{
		try {
			terminate();
			badCore::CharBuffer password;
			sql::mysql::MySQL_Driver* driver = sql::mysql::get_driver_instance();
			mConnect.reset(driver->connect(service, user, password.data()));
		}
		catch (const sql::SQLException& e) {
			terminate();
			return badCore::BoolMessage::failure(e.what());
		}

		return badCore::BoolMessage::success();
	}

	badCore::BoolMessage SQLInserter::set_auto_commit(bool state)noexcept
	{
		if (!is_connected()) {
			terminate();
			return badCore::BoolMessage::failure("No Connection");
		}
	
		try {
			mConnect->setAutoCommit(state);
		}
		catch (const sql::SQLException& e) {
			terminate();
			return badCore::BoolMessage::failure(std::string("Connection terminated: ") + e.what());
		}
	
		return badCore::BoolMessage::success("State set to: " + std::to_string(static_cast<int>(state)));
	}

	badCore::BoolMessage SQLInserter::commit() noexcept
	{
		if (!is_connected()) {
			terminate();
			return badCore::BoolMessage::failure("No Connection");
		}
	
		try {
			mConnect->commit();
		}
		catch (const sql::SQLException& e)
		{
	
			std::string rollback_throw;
			try {
				mConnect->rollback();
			}
			catch (const sql::SQLException& e2) {
				rollback_throw = e2.what();
			}
	
			terminate();
			return badCore::BoolMessage::failure(std::string("Connection terminated: ") + e.what() + " " + rollback_throw);
		}
	
		return badCore::BoolMessage::success();
	}

	sql::PreparedStatement* SQLInserter::get_pstmt(const std::string& sql)
	{
		auto it = cache.find(sql);

		if (it != cache.end()) {
			return it->second.get();
		}

		auto new_pstmt = std::unique_ptr<sql::PreparedStatement>(mConnect.get()->prepareStatement(sql));

		auto ptr = new_pstmt.get();

		cache.emplace(sql, std::move(new_pstmt));

		return ptr;
	}
}