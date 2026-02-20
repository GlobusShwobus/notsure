#include "SQL_inserter.h"
#include "CharBuffer.h"

namespace badSQL
{
	using badCore::bString;

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
	bString SQLInserter::connect(const std::string& user, const std::string& service) noexcept
	{
		try {
			terminate();
			badCore::CharBuffer password;
			sql::mysql::MySQL_Driver* driver = sql::mysql::get_driver_instance();
			mConnect.reset(driver->connect(service, user, password.data()));
		}
		catch (const sql::SQLException& e) {
			terminate();
			return bString::failure(e.what());
		}

		return bString::success();
	}

	bString SQLInserter::set_auto_commit(bool state)noexcept
	{
		if (!is_connected()) {
			terminate();
			return bString::failure("No Connection");
		}
	
		try {
			mConnect->setAutoCommit(state);
		}
		catch (const sql::SQLException& e) {
			terminate();
			return bString::failure(std::string("Connection terminated: ") + e.what());
		}
	
		return bString::success();
	}

	bString SQLInserter::commit() noexcept
	{
		if (!is_connected()) {
			terminate();
			return bString::failure("No Connection");
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
			return bString::failure(std::string("Connection terminated: ") + e.what() + " " + rollback_throw);
		}
	
		return bString::success();
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