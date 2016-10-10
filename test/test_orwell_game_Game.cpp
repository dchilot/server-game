#include "orwell/game/Game.hpp"
#include "orwell/game/Team.hpp"
#include "orwell/game/Ruleset.hpp"
#include "orwell/Server.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>

#include <iostream>
#include <unistd.h>
#include <cstdint>

#include <log4cxx/ndc.h>

//#include <gtest/gtest.hpp>

#include "orwell/support/GlobalLogger.hpp"

#include "Common.hpp"


class TestOrwellGameGame : public ::testing::Test
{
protected:
	TestOrwellGameGame()
		: m_gameDuration(60)
		, m_ticDuration(120)
		, m_agentUrl("tcp://localhost:9003")
		, m_pullUrl("tcp://localhost:9001")
		, m_publishUrl("tcp://localhost:9000")
		, m_game(
				m_fakeSystemProxy,
				boost::posix_time::time_duration(0, 0, m_gameDuration),
				m_ruleset,
				m_server)
	{
	}

	virtual void SetUp()
	{
	}

	virtual void TearDown()
	{
	}

	uint32_t const m_gameDuration;
	long const m_ticDuration;
	FakeSystemProxy m_fakeSystemProxy;
	FakeAgentProxy m_fakeAgentProxy;
	orwell::game::Ruleset m_ruleset;
	std::string const m_agentUrl;
	std::string const m_pullUrl;
	std::string const m_publishUrl;
	FakeServer m_server;
	orwell::game::Game m_game;
};


TEST_F(TestOrwellGameGame, Create)
{
	EXPECT_FALSE(m_game.getIsRunning())
		<< "The game is not running when created.";
	EXPECT_EQ(m_gameDuration, m_game.getSecondsLeft())
		<< "No seconds have been consummed yet.";
}

int main(int argc, char ** argv)
{
	orwell::support::GlobalLogger::Create("test_orwell_game_Game", "test_orwell_game_Game.log", true);
	log4cxx::NDC ndc("test_orwell_game_Game");
	ORWELL_LOG_INFO("Test starts\n");
	::testing::InitGoogleTest(&argc, argv);
	int aResult = RUN_ALL_TESTS();
	orwell::support::GlobalLogger::Clear();
	return aResult;
}
