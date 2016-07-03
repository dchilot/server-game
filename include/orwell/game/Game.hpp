#pragma once

#include <map>
#include <set>
#include <memory>

#include <chrono>
#include <boost/optional.hpp>

#include "orwell/game/Player.hpp"
#include "orwell/game/Team.hpp"
#include "orwell/game/Landmark.hpp"

#include "server-game.pb.h"

namespace orwell
{
class Server;

namespace com
{
class Sender;
} // com
namespace game
{
class Robot;
class Item;
class Contact;
class Ruleset;

class Game
{
public:
	Game(
			std::chrono::steady_clock::duration const & iGameDuration,
			Ruleset const & iRuleset,
			Server & ioServer);
	~Game();

//	std::shared_ptr< com::Sender > getPublisher();

	std::shared_ptr<Robot> accessRobot(std::string const & iRobotName);
	bool getHasRobotById(std::string const & iRobotId) const;
	std::map<std::string, std::shared_ptr<Robot> > const & getRobots() const;

	std::shared_ptr< Player > accessPlayer(std::string const & iPlayerName);
	std::map< std::string, std::shared_ptr< Player > > const & getPlayers();

	bool getIsRunning() const;

	uint64_t getSecondsLeft() const;

	void start();
	void stop();

	//add empty PlayerContext
	bool addPlayer(std::string const & iName);

	/// Remove a player named #iName if found.
	/// \param iName
	///  The name of the player to remove.
	/// \return
	///  True if and only if the player was found and removed.
	bool removePlayer(std::string const & iName);

	/// Add a team by name.
	/// \param iTeamName
	///  The name of the team to add.
	/// \return
	///  True if and only if the team has been added (it can fail if a team
	///  with the same name is already present).
	bool addTeam(std::string const & iTeamName);

	/// Remove a team.
	/// \param iTeamName
	///  The name of the team to remove.
	/// \return
	///  True if and only if the team has been removed (it can fail if no
	///  team matching the name has been found). WARNING: we do not check
	///  if robots are still in the team (yet).
	bool removeTeam(std::string const & iTeamName);

	/// Append the names of the teams to a provided array.
	/// \param ioTeams
	///  What to add the names to.
	void getTeams(std::vector< std::string > & ioTeams) const;

	/// Get a team from its name.
	/// \param iTeamName
	///  The name of the team to retrieve.
	/// \return
	///  The team if found and the neutral team otherwise.
	Team const & getTeam(std::string const & iTeamName) const;

	boost::optional< std::string > const & getWinner() const;

	/// Add a robot.
	/// \param iName
	///  The name of the robot to add (this should be unique). If a robot with
	///  the same name is found this call is ignored and no new robot is created.
	/// \param iTeamName
	///  The addition will only work with a valid team name.
	/// \param iVideoRetransmissionPort
	///  The port used to send the video from the camera on the robot.
	/// \param iServerCommandPort
	///  The port used to receive commands related to the video transmission.
	/// \return
	///  True if and only if the robot was added successfully.
	bool addRobot(
			std::string const & iName,
			std::string const & iTeamName,
			uint16_t const iVideoRetransmissionPort,
			uint16_t const iServerCommandPort,
			std::string iRobotId = "");

	/// Remove a robot named #iName if found.
	/// \param iName
	///  The name of the robot to remove.
	/// \return
	///  True if and only if the robot was found and removed.
	bool removeRobot(std::string const & iName);

	void fire(std::string const & iRobotId);

	/// Perform actions that need to be performed every tic
	void step();

	std::shared_ptr< Robot > getRobotWithoutRealRobot(
			std::string const & iTemporaryRobotId) const;
	std::shared_ptr< Robot > getRobotForPlayer(std::string const & iPlayer) const;
	std::shared_ptr<Robot> getAvailableRobot() const;
	void fillGameStateMessage(messages::GameState & oGameState);

	void robotIsInContactWith(
			std::string const & iRobotId,
			std::shared_ptr<Item> const iItem);
	void robotDropsContactWith(
			std::string const & iRobotId,
			std::shared_ptr<Item> const iItem);

	void setTime(std::chrono::steady_clock::time_point const & iCurrentTime);

	void stopIfGameIsFinished();

	void setMapLimits(std::vector< orwell::game::Landmark > const & iMapLimits);

	std::vector< orwell::game::Landmark > const & getMapLimits() const;
private:
	/// \return
	///  A RobotID that is not already used.
	std::string getNewRobotId() const;

	void readImages();

	/// Loop over the different contacts and give them the new time.
	void handleContacts();

	/// True if and only if the game is running
	bool m_isRunning;
	/// Each connected robot has a robotContext in this map. The key is the robot name.
	std::map<std::string, std::shared_ptr<Robot> > m_robots;
	/// Same as #m_robots except that the kid is the ID instead of the name
	std::map< std::string, std::shared_ptr< Robot > > m_robotsById;
	/// Each connected controller has a playerContext in this map. The key is the player name.
	std::map< std::string, std::shared_ptr< Player > > m_players;
	/// Each connected controller has a playerContext in this map. The key is the team name.
	std::map<std::string, Team> m_teams;
	/// Stores the temporary robots ID sent by the different robots in case
	/// they send the same value again to send back the same information
	/// the following times.
	mutable std::map< std::string, std::string > m_registeredRobots;
	/// stores the temp files containing the pids of the webservers, to kill them later
	std::vector<std::string> m_tmpFiles;
	std::chrono::steady_clock::time_point m_time;
	std::chrono::steady_clock::time_point m_startTime;
	std::chrono::steady_clock::duration m_gameDuration;

	//Contacts between robots and flags. The key is the robotId
	std::map<std::string, std::unique_ptr<Contact> > m_contacts;

	Server & m_server;
	/// robot ids for which an image has been requested
	std::set< std::string > m_pendingImage;

	boost::optional< std::string > m_winner;

	Ruleset const & m_ruleset;
	std::vector< orwell::game::Landmark > m_mapLimits;
};

} // game
} // orwell

