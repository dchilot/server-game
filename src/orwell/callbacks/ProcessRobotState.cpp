#include "orwell/callbacks/ProcessRobotState.hpp"

#include <set>

#include "orwell/support/GlobalLogger.hpp"
#include "orwell/game/Game.hpp"
#include "orwell/game/Robot.hpp"
#include "orwell/com/RawMessage.hpp"
#include "orwell/com/Sender.hpp"
#include "orwell/game/Item.hpp"
#include "orwell/game/ItemEncoder.hpp"

#include "controller.pb.h"
#include "server-game.pb.h"
#include "robot.pb.h"

using orwell::messages::ServerRobotState;
using orwell::messages::PlayerState;
using orwell::messages::Item;
using orwell::com::RawMessage;

namespace orwell
{
namespace callbacks
{

ProcessRobotState::ProcessRobotState(
		std::shared_ptr< com::Sender > ioPublisher,
		game::Game & ioGame)
	: InterfaceProcess(ioPublisher, ioGame)
{
}

void ProcessRobotState::execute()
{
	std::string const & aDestination = getArgument("RoutingID");
	if (not m_game->getHasRobotById(aDestination))
	{
		ORWELL_LOG_WARN("This is an invalid destination: " << aDestination);
		return;
	}
	orwell::messages::ServerRobotState const & aRobotStateMsg = static_cast< orwell::messages::ServerRobotState const & >(*m_msg);

	//ORWELL_LOG_INFO("ProcessRobotState::execute : simple relay");

	std::set< std::shared_ptr< orwell::game::Item > > aVisitedItems;

	for (int i = 0; i < aRobotStateMsg.rfid_size() ; ++i)
	{
		std::shared_ptr< orwell::game::Item > aItem = game::Item::GetItemByRfid(aRobotStateMsg.rfid(i).rfid());
		if (not aItem)
		{
			continue;
		}
		aVisitedItems.insert(aItem);
		switch (aRobotStateMsg.rfid(i).status())
		{
		case messages::Status::ON :
			ORWELL_LOG_INFO("Robot " << aDestination <<
					" records contact with RFID " << aRobotStateMsg.rfid(i).rfid() <<
					" at " << aRobotStateMsg.rfid(i).timestamp());
			m_game->robotIsInContactWith(aDestination, aItem);
			break;
		case messages::Status::OFF :
			ORWELL_LOG_INFO("Robot " << aDestination <<
					" stops contact with RFID " << aRobotStateMsg.rfid(i).rfid() <<
					" at " << aRobotStateMsg.rfid(i).timestamp());
			m_game->robotDropsContactWith(aDestination, aItem);
			break;
		}
	}
	for (int i = 0; i < aRobotStateMsg.colour_size() ; ++i)
	{
		std::shared_ptr< orwell::game::Item > aItem = game::Item::GetItemByColour(aRobotStateMsg.colour(i).colour());
		if (not aItem)
		{
			continue;
		}
		aVisitedItems.insert(aItem);
		switch (aRobotStateMsg.colour(i).status())
		{
		case messages::Status::ON :
			ORWELL_LOG_INFO("Robot " << aDestination <<
					" records contact with Colour tag " << aRobotStateMsg.colour(i).colour() <<
					" at " << aRobotStateMsg.colour(i).timestamp());
			m_game->robotIsInContactWith(aDestination, aItem);
			break;
		case messages::Status::OFF :
			ORWELL_LOG_INFO("Robot " << aDestination <<
					" stops contact with Colour tag " << aRobotStateMsg.colour(i).colour() <<
					" at " << aRobotStateMsg.colour(i).timestamp());
			m_game->robotDropsContactWith(aDestination, aItem);
			break;
		}
	}

	//todo : what do we forward to the player ?
	// forward this message to each controler
	//RawMessage aForward(aDestination, "RobotState", aRobotStateMsg.SerializeAsString());
	//m_publisher->send( aForward );
	if (aVisitedItems.size() > 1)
	{
		ORWELL_LOG_WARN(
				"RobotState contained information about "
				<< aVisitedItems.size() << " sensors for robot with routing id "
				<< aDestination << ".");
	}
	for (std::shared_ptr< orwell::game::Item > aItem: aVisitedItems)
	{
		PlayerState aPlayerState;
		Item * aPbItem = aPlayerState.mutable_item();
		assert(nullptr != aPbItem);
		aItem->getEncoder()->encode(*aPbItem);
		RawMessage aMessage(
				aDestination,
				"PlayerState",
				aPlayerState.SerializeAsString());
		m_publisher->send(aMessage);
	}
}

}
}
