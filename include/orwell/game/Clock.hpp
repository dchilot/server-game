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

	private :
		static time_point s_fakeNow;
		static bool s_autoIncrement;
		static duration s_autoIncrementValue;
	};
};
} // game
} // orwell
