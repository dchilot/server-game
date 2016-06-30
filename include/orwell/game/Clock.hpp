#pragma once

#include <chrono>

namespace orwell
{
namespace game
{
struct production
{
	using clock = std::chrono::steady_clock;
};


struct mock
{
	struct clock : std::chrono::steady_clock
	{
		static time_point now();

		static void setAutoIncrement(duration const & iAutoIncrementValue);

		static void disableAutoIncrement();

		static time_point fake_now;
		static bool auto_increment;
		static duration auto_increment_value;
	};
};
} // game
} // orwell
