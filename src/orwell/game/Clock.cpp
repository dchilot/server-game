#include "orwell/game/Clock.hpp"

#include "orwell/support/GlobalLogger.hpp"

namespace orwell
{
namespace game
{

mock::clock::time_point mock::clock::s_fakeNow = ::std::chrono::steady_clock::now();
bool mock::clock::s_autoIncrement = false;
mock::clock::duration mock::clock::s_autoIncrementValue = mock::clock::duration(0);

mock::clock::time_point mock::clock::now()
{
	if (s_autoIncrement)
	{
		s_fakeNow += s_autoIncrementValue;
	}
	return s_fakeNow;
}

void mock::clock::setAutoIncrement(duration const & iAutoIncrementValue)
{
	s_autoIncrement = true;
	s_autoIncrementValue = iAutoIncrementValue;
}

void mock::clock::disableAutoIncrement()
{
	s_autoIncrement = false;
}

void mock::clock::forceNow(time_point const & iNewNow)
{
	s_fakeNow = iNewNow;
}

} // game
} // orwell

