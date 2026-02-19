#pragma once
#include <string>
#include "Sequence.h"

namespace badCurl
{
	struct WRITE_HANDLE {
		badCore::Sequence<char> data;
		long httpcode = -1;
		bool is_good = false;
		std::string contentType;
	};
}