#pragma once

#include <string_view>
#include <string>
#include "Sequence.h"

namespace badSQL
{
	struct StatementTemplate
	{
		StatementTemplate(std::string_view schema, std::string_view table)
			:schema_name(schema), table_name(table)
		{
		}

		std::string schema_name;
		std::string table_name;
		badCore::Sequence<std::string> fields;
	};


	struct FinalizedStatement
	{
		explicit FinalizedStatement(const StatementTemplate& build, std::size_t repeat = 1);

		std::string statement;
	};
}