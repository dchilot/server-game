#include "orwell/game/Contact.hpp"

#include <chrono>

#include "orwell/support/GlobalLogger.hpp"
#include "orwell/game/Item.hpp"
#include "orwell/game/Robot.hpp"
#include "orwell/game/Team.hpp"

namespace orwell
{
namespace game
{

Contact::Contact(
		std::chrono::steady_clock::time_point const & iStartTime,
		std::chrono::steady_clock::duration const & iTimerDuration,
		std::shared_ptr<Robot> iRobot,
		std::shared_ptr<Item> iItem)
	: m_robot(iRobot)
	, m_item(iItem)
	, m_stopTime(iStartTime + iTimerDuration)
{
	ORWELL_LOG_DEBUG("Create a contact");
}

Contact::~Contact()
{
}

StepSignal Contact::step(
		std::chrono::steady_clock::time_point const & iCurrentTime)
{
	StepSignal aResult = StepSignal::SILENCEIKILLU;
	ORWELL_LOG_DEBUG("Step in a contact");
	if (iCurrentTime > m_stopTime)
	{
		m_robot->getTeam().captureItem(m_item);
	}
	else
	{
		aResult = StepSignal::AH_AH_AH_AH_STAYINGALIVE;
	}
	return aResult;
}

} // game
} // orwell

