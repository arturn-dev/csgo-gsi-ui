#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <string>
#include <vector>

class GameState
{
public:
	struct Provider
	{
		std::string name;
		unsigned long appId;
		unsigned long version;
		std::string steamId;
		unsigned long timestamp;
	};

	enum Team
	{
		TEAM_UNKNOWN,
		T,
		CT
	};

	struct Weapon
	{
		std::string name;
		std::string paintKit;

		enum Type
		{
			WEAPON_TYPE_UNKNOWN,
			KNIFE,
			PISTOL,
			RIFLE,
			GRENADE
		} type;

		int ammoClip;
		int ammoClipMax;
		int ammoReserve;

		enum State
		{
			WEAPON_STATE_UNKNOWN,
			HOLSTERED,
			ACTIVE
		} state;
	};

	struct Vec3
	{
		int x, y, z;
	};

	struct Player
	{
		std::string steamId;
		std::string name;
		unsigned int observerSlot;
		Team team;
		std::string activity;

		struct RoundState
		{
			int health;
			int armor;
			bool helmet;
			int flashed;
			int smoked;
			int burning;
			int money;
			int kills;
			int killHeadshots;
			int totalDamage;
			int equipmentValue;
		} roundState;

		struct MatchStats
		{
			int kills;
			int assists;
			int deaths;
			int mvps;
			int score;
		} matchStats;

		std::vector<Weapon> weapons;
		std::string specTarget;
		Vec3 position;
		Vec3 forward;
	};

	enum GamePhase
	{
		GAME_PHASE_UNKNOWN,
		LIVE
	};

	struct MapInfo
	{
		enum Mode
		{
			MAP_MODE_UNKNOWN,
			COMPETITIVE
		} mode;

		std::string name;

		GamePhase phase;

		int roundNo;

		struct TeamStats
		{
			int score;
			int consecutiveRoundLosses;
			int timeoutsRemaining;
			int matchesWonThisSeries;
		} teamCtStats, teamTStats;

		int numberOfMatchesToWinSeries;
		int currentSpectatorsCount;
		int souvenirsTotal;

		enum RoundWinCause
		{
			MAP_ROUND_WIN_CAUSE_UNKNOWN,
			CT_WIN_ELIMINATION,
			CT_WIN_DEFUSE,
			T_WIN_ELIMINATION
		};
		std::vector<RoundWinCause> roundWins;
	};

	struct BombInfo
	{
		enum BombState
		{
			BOMB_STATE_UNKNOWN,
			PLANTED
		} bombState;
		Vec3 position;
		float countdown;
	};

private:
	Provider provider;
	const Player& currentPlayer;
	MapInfo mapInfo;
	std::vector<Player> players;
	BombInfo bombInfo;
	// TODO: grenades, round?, phase_countdown?
};

#endif //GAMESTATE_H
