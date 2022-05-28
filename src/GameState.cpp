#include "GameState.h"

GameState::MapInfo::SideStats& GameState::MapInfo::SideStats::operator=(const GameState::MapInfo::SideStats& other)
{
	score = other.score;
	consecutiveRoundLosses = other.consecutiveRoundLosses;
	timeoutsRemaining = other.timeoutsRemaining;
	matchesWonThisSeries = other.matchesWonThisSeries;

	return *this;
}

