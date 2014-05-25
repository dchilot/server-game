/* This class stores the information about a robot that is connected to the server */

#pragma once

#include <string>

#include <memory>

namespace orwell {
namespace messages {
class RobotState;
}
namespace game
{
class Player;

class Robot
{
public:
	Robot(
			std::string const & iName,
			std::string const & iRobotId);
	~Robot();

	void setHasRealRobot(bool const iHasRealRobot);
	bool const getHasRealRobot() const;

	void setPlayer(std::shared_ptr< Player > const iPlayer);
	std::shared_ptr< Player > const getPlayer() const;
	bool const getHasPlayer() const;

	void setVideoAddress(std::string const & iVideoAddress);
	std::string const & getVideoAddress() const;

	void setVideoPort(uint32_t const iVideoPort);
	uint32_t getVideoPort() const;

	std::string const & getName() const;
	std::string const & getRobotId() const;

	bool const getIsAvailable() const;

	void fillRobotStateMessage( messages::RobotState & oMessage );

private:
	std::string m_name;
	std::string m_robotId;
	std::string m_videoAddress;
	uint32_t m_videoPort;
	bool m_hasRealRobot;
	std::weak_ptr< Player > m_player;
};

}} //end namespace
