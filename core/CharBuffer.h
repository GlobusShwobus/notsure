#pragma once

#include "Sequence.h"

namespace badCore
{
	class CharBuffer final
	{
	public:
		CharBuffer();
		CharBuffer(const CharBuffer&) = delete;
		CharBuffer& operator=(const CharBuffer&) = delete;
		CharBuffer(CharBuffer&&) = delete;
		CharBuffer& operator=(CharBuffer&&) = delete;

		char* data() noexcept;

		std::size_t size()const noexcept;

		~CharBuffer();

	private:
		Sequence<char> buffer;
	};
}