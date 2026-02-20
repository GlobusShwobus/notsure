#pragma once

#include <string_view>
#include <string>
#include "Sequence.h"

namespace badSQL
{
	struct TableTemplate
	{
		TableTemplate(std::string_view schema, std::string_view table)
			:schema_name(schema), table_name(table)
		{
		}

		std::string schema_name;
		std::string table_name;
		badCore::Sequence<std::string> fields;
	};

	std::string parse_to_sql_insert_statement(const TableTemplate& build, std::size_t repeat = 1);
}