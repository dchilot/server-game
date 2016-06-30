#include "orwell/game/Clock.hpp"

#include "orwell/support/GlobalLogger.hpp"

namespace orwell
{
namespace game
{

mock::clock::time_point mock::clock::fake_now = ::std::chrono::steady_clock::now();
bool mock::clock::auto_increment = false;
mock::clock::duration mock::clock::auto_increment_value = mock::clock::duration(0);

mock::clock::time_point mock::clock::now()
{
	if (auto_increment)
	{
		fake_now += auto_increment_value;
	}
	return fake_now;
}

void mock::clock::setAutoIncrement(duration const & iAutoIncrementValue)
{
	auto_increment = true;
	auto_increment_value = iAutoIncrementValue;
}

void mock::clock::disableAutoIncrement()
{
	auto_increment = false;
}

} // game
} // orwell

