
#pragma once

#include <string>
#include <stdint.h>

#include <boost/property_tree/ptree_fwd.hpp>
#include <chrono>

namespace orwell
{
namespace game
{

class Ruleset
{
public :

	Ruleset();

	void parseConfig(
			std::string const & iRulesetName,
			boost::property_tree::ptree const & iPtree);

	std::string m_gameName;
	uint32_t m_scoreToWin;
	uint32_t m_pointsOnCapture;
	std::chrono::milliseconds m_timeToCapture;

};
} // game
} // orwell

