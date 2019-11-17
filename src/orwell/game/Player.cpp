#include "orwell/game/Player.hpp"

namespace orwell
{
namespace game
{

Player::Player(std::string const & iName) : m_name(iName)
{
}

Player::~Player()
{
}

void Player::setRobot(std::shared_ptr< Robot > aRobot)
{
	m_robot = aRobot;
}

std::string const & Player::getName() const
{
	return m_name;
}

std::shared_ptr< Robot > Player::getRobot() const
{
	return m_robot.lock();
}

bool Player::getHasRobot() const
{
	return not m_robot.expired();
}


}
}
