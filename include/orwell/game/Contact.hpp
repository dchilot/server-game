#pragma once
#include "orwell/game/TimeBound.hpp"

#include <memory>
#include <chrono>

namespace orwell
{
namespace game
{
class Item;
class Robot;

class Contact : public TimeBound
{
public:
	Contact(
			std::chrono::steady_clock::time_point const & iStartTime,
			std::chrono::steady_clock::duration const & iTimerDuration,
			std::shared_ptr<Robot> iRobot,
			std::shared_ptr<Item> iItem);
	~Contact();

	StepSignal step(
			std::chrono::steady_clock::time_point const & iCurrentTime)
		override;

private:
	std::shared_ptr<Robot> m_robot;
	std::shared_ptr<Item> m_item;
	std::chrono::steady_clock::time_point const m_stopTime;
};

}
} //end namespace

