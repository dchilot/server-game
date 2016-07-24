/// This class stores the information about a robot that is connected to the server

#pragma once

#include <string>
#include <memory>

#include <zmq.hpp>

#include "orwell/com/Socket.hpp"

namespace orwell
{

namespace game
{
class Player;
class Item;
class Team;

class Robot
{
public:
	Robot(
			std::string const & iName,
			std::string const & iRobotId,
			Team & iTeam,
			uint16_t const & iVideoRetransmissionPort,
			uint16_t const & iServerCommandPort);
	~Robot();

	Team & getTeam();

	Team const & getTeam() const;

	void setHasRealRobot(bool const iHasRealRobot);
	bool const getHasRealRobot() const;

	void setPlayer(std::shared_ptr< Player > const iPlayer);
	std::shared_ptr< Player > const getPlayer() const;
	bool const getHasPlayer() const;

	void setVideoUrl(std::string const & iVideoUrl);
	std::string const & getVideoUrl() const;

	uint16_t getVideoRetransmissionPort() const;

	uint16_t getServerCommandPort() const;

//	void setVideoAddress(std::string const & iVideoAddress);
//	std::string const & getVideoAddress() const;
//
//	void setVideoPort(uint32_t const iVideoPort);
//	uint32_t getVideoPort() const;

	std::string const & getName() const;
	std::string const & getRobotId() const;

	bool const getIsAvailable() const;

	void fire();
	void stop();

	void readImage();

	void startVideo();
//	void fillRobotStateMessage( messages::RobotState & oMessage );

private:
	std::string m_name;
	std::string m_robotId;
	Team & m_team;
	std::string m_videoUrl; //the origin URL of the videofeed
	uint16_t m_videoRetransmissionPort; // the port on which the python server retransmits the video feed
	uint16_t m_serverCommandPort; // the port used to give instructions to the retransmitter.
	bool m_hasRealRobot;
	std::weak_ptr< Player > m_player;
	zmq::context_t m_zmqContext;
	orwell::com::Socket m_serverCommandSocket;
	/// true if and only if the robot is waiting for an answer to a capture
	/// command that will contain an image
	bool m_pendingImage;
	std::string m_tempFile;
};

}} //end namespace

