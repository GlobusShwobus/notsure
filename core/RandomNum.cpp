#include "pch.h"
#include "RandomNum.h"
#include <assert.h>

namespace badCore
{
	RandomNum::RandomNum()
	{
		rng = std::make_unique<std::mt19937>(std::random_device{}());
	}

	int RandomNum::rInt(int min, int max)const noexcept
	{
		assert(min <= max && "Invalid logic: min is more than max");
		return std::uniform_int_distribution<int>(min, max)(*rng);
	}

	float RandomNum::rFloat(float min, float max)const noexcept
	{
		assert(min <= max && "Invalid logic: min is more than max");
		return std::uniform_real_distribution<float>(min, max)(*rng);
	}
}