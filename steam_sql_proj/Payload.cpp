#include "Payload.h"

namespace badSQL
{
	void user_bind(sql::PreparedStatement* pstmt, const Payload& item)
	{
		pstmt->setInt64(1, *item.recipient_id);
		pstmt->setString(2, *item.label);
		pstmt->setString(3, item.data->dump());
	}

	void user_bind_bulk(sql::PreparedStatement* pstmt, const Payload& item, std::size_t& index)
	{
		pstmt->setInt64 (index++, *item.recipient_id);
		pstmt->setString(index++, *item.label);
		pstmt->setString(index++, item.data->dump());
	}
}