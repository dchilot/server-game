#include "orwell/callbacks/ProcessTimer.hpp"

#include "orwell/support/GlobalLogger.hpp"
#include "orwell/com/RawMessage.hpp"
#include "orwell/game/Game.hpp"
#include "orwell/game/Item.hpp"
#include "orwell/game/Robot.hpp"
#include "orwell/game/ItemEncoder.hpp"
#include "orwell/game/Landmark.hpp"
#include "orwell/com/Sender.hpp"

#include "controller.pb.h"
#include "server-game.pb.h"

using orwell::com::RawMessage;
using orwell::messages::GameState;

namespace orwell
{
namespace callbacks
{

ProcessTimer::ProcessTimer(
		std::shared_ptr< com::Sender > ioPublisher,
		game::Game & ioGame)
	: InterfaceProcess(ioPublisher, ioGame)
{
}

void ProcessTimer::execute()
{
	ORWELL_LOG_TRACE("ProcessTimer::execute : broadcast Gamestate");

	GameState aGameState;
	ORWELL_LOG_DEBUG("Is game running ? " << m_game->getIsRunning());
	aGameState.set_playing(m_game->getIsRunning());
	aGameState.set_seconds(m_game->getSecondsLeft());
	for (game::Landmark const & aLandmark:  m_game->getMapLimits())
	{
		messages::Landmark * aMapLimit = aGameState.add_map_limits();
		messages::Coordinates * aPosition = aMapLimit->mutable_position();
		aPosition->set_x(aLandmark.getPosition().getX());
		aPosition->set_y(aLandmark.getPosition().getY());
		messages::RGBColour * aColour = aMapLimit->mutable_colour();
		aColour->set_r(aLandmark.getColour().getRed());
		aColour->set_g(aLandmark.getColour().getGreen());
		aColour->set_b(aLandmark.getColour().getBlue());
	}
	for (auto const aItem: game::Item::GetAllItems())
	{
		messages::Item * aMessageItem = aGameState.add_items();
		aItem->getEncoder()->encode(*aMessageItem);
	}
	if (m_game->getWinner())
	{
		aGameState.set_winner(*m_game->getWinner());
		std::map< std::string, std::shared_ptr< orwell::game::Robot > > aRobots =
			m_game->getRobots();
		for (auto const & aPair : aRobots)
		{
			std::string aRobotRoutingId = aPair.first;
			auto aRobot = aPair.second;
			ORWELL_LOG_INFO("Ask robot " << aRobot->getName() << " to stop moving");
			orwell::messages::Input aInput;
			aInput.mutable_move()->set_left(0);
			aInput.mutable_move()->set_right(0);
			aInput.mutable_fire()->set_weapon1(false);
			aInput.mutable_fire()->set_weapon2(false);
			RawMessage aInpurMessage(aRobotRoutingId, "Input", aInput.SerializeAsString());
			m_publisher->send(aInpurMessage);
		}
	}

	RawMessage aMessage("all_clients", "GameState", aGameState.SerializeAsString());
	m_publisher->send(aMessage);
}

} // namespace callbacks
} // namespace orwell
