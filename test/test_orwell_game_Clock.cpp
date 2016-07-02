#include "orwell/game/Clock.hpp"

#include <iostream>
#include <unistd.h>
#include <cstdint>

#include <log4cxx/ndc.h>

#include "orwell/support/GlobalLogger.hpp"

#include "Common.hpp"


class Failer
{
public :
	Failer();

	void addFailure();

	uint64_t getFailures() const;
private :
	uint64_t m_failures;
};

Failer::Failer()
	: m_failures(0)
{
}

void Failer::addFailure()
{
	++m_failures;
}

uint64_t Failer::getFailures() const
{
	return m_failures;
}

#define ORWELL_FAIL(Expected, Received, Message, Failer) \
{\
	if (Expected != Received)\
	{\
		ORWELL_LOG_ERROR("expected: " << Expected);\
		ORWELL_LOG_ERROR("received: " << Received);\
		ORWELL_LOG_ERROR(Message);\
		Failer.addFailure();\
	}\
}\

#define ORWELL_FAIL_TRUE(Condition, Message, Failer) \
{\
	if (!Condition)\
	{\
		ORWELL_LOG_ERROR(Message);\
		Failer.addFailure();\
	}\
}\

void test_Clock_now_identical(Failer & ioFailer)
{
	log4cxx::NDC aLocalNDC("test_Clock_now_identical");
	auto aFirstNow = orwell::game::mock::clock::now();
	usleep(10);
	auto aSecondNow = orwell::game::mock::clock::now();
	ORWELL_FAIL_TRUE(
			(aFirstNow == aSecondNow),
			"The mock clock should stay at a given time.",
			ioFailer);
}

void test_Clock_auto_increment(Failer & ioFailer)
{
	log4cxx::NDC aLocalNDC("test_Clock_auto_increment");
	auto const kDelta = std::chrono::milliseconds(5);
	orwell::game::mock::clock::setAutoIncrement(kDelta);
	auto aFirstNow = orwell::game::mock::clock::now();
	auto aSecondNow = orwell::game::mock::clock::now();
	ORWELL_FAIL_TRUE(
			(aFirstNow < aSecondNow),
			"Auto incrementing",
			ioFailer);
	ORWELL_FAIL_TRUE(
			(kDelta == (aSecondNow - aFirstNow)),
			"Check exact value",
			ioFailer);
	orwell::game::mock::clock::disableAutoIncrement();
	auto aThirdNow = orwell::game::mock::clock::now();
	ORWELL_FAIL_TRUE(
			(aSecondNow == aThirdNow),
			"No more auto incrementing",
			ioFailer);
}

void test_Clock_force_now(Failer & ioFailer)
{
	log4cxx::NDC aLocalNDC("test_Clock_force_now");
	auto const kDelta = std::chrono::milliseconds(5);
	auto aFirstNow = orwell::game::mock::clock::now();
	orwell::game::mock::clock::forceNow(aFirstNow + kDelta);
	usleep(10);
	auto aSecondNow = orwell::game::mock::clock::now();
	ORWELL_FAIL_TRUE(
			(aFirstNow + kDelta == aSecondNow),
			"The value of now should be the new one",
			ioFailer);
}

int main()
{
	orwell::support::GlobalLogger::Create("test_orwell_game_Clock", "test_orwell_game_Clock.log", true);
	log4cxx::NDC ndc("test_orwell_game_Clock");
	ORWELL_LOG_INFO("Test starts\n");

	Failer aFailer;

	test_Clock_now_identical(aFailer);
	test_Clock_auto_increment(aFailer);
	test_Clock_force_now(aFailer);

	orwell::support::GlobalLogger::Clear();
	return aFailer.getFailures();
}
