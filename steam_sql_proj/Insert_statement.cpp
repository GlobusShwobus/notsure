#include "Insert_statement.h"
#include <format>

namespace badSQL
{
    FinalizedStatement::FinalizedStatement(const StatementTemplate& build, std::size_t repeat)
    {
        statement.reserve(128 + build.fields.size() * repeat * 4);

        std::format_to(std::back_inserter(statement),
            "INSERT INTO {}.{} (",
            build.schema_name,
            build.table_name);

        for (size_t i = 0; i < build.fields.size(); ++i) {
            if (i) statement += ',';
            statement += build.fields[i];
        }

        statement += ") VALUES ";

        for (size_t i = 0; i < repeat; ++i) {
            if (i) statement += ',';

            statement += '(';

            for (size_t j = 0; j < build.fields.size(); ++j) {
                if (j) statement += ',';
                statement += '?';
            }

            statement += ')';
        }

        statement += ';';
    }
}