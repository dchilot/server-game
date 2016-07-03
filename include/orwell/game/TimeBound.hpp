#pragma once

#include <chrono>

namespace orwell
{
namespace game
{

enum class StepSignal
{
	AH_AH_AH_AH_STAYINGALIVE,
	SILENCEIKILLU
};

class TimeBound
{
public:
	virtual StepSignal step(
			std::chrono::steady_clock::time_point const & iCurrentTime) = 0;

protected :
	TimeBound();
	virtual ~TimeBound();
};

}
} //end namespace

