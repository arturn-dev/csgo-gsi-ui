#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <string>
#include <utility>
#include <vector>
#include <list>

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

	enum Side
	{
		SIDE_UNKNOWN,
		T_SIDE,
		CT_SIDE
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

		// optional values (-1 if undefined)
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
		double x, y, z;
	};

	struct Player
	{
		std::string steamId;
		std::string name;
		unsigned int observerSlot;
		Side team;

		struct
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

		struct
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

		struct SideStats
		{
			Side side;
			int score;
			int consecutiveRoundLosses;
			int timeoutsRemaining;
			int matchesWonThisSeries;
		} ctSideStats, tSideStats;

		int numberOfMatchesToWinSeries;
		int currentSpectatorsCount;
		int souvenirsTotal;

		enum RoundWinCause
		{
			MAP_ROUND_WIN_CAUSE_UNKNOWN,
			CT_WIN_ELIMINATION,
			CT_WIN_DEFUSE,
			T_WIN_ELIMINATION,
			T_WIN_BOMB
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
		double countdown;
	};

private:
	Provider provider;
	const Player& currentPlayer;
	MapInfo mapInfo;
	std::list<Player> players;
	BombInfo bombInfo;
	// TODO: grenades, round?, phase_countdown?

public:
	GameState(Provider provider, const Player& currentPlayer, MapInfo mapInfo,
			  std::list<Player> players, BombInfo bombInfo) : provider(std::move(provider)),
															  currentPlayer(currentPlayer),
															  mapInfo(std::move(mapInfo)),
															  players(std::move(players)),
															  bombInfo(bombInfo)
	{}

	const Provider& getProvider() const
	{
		return provider;
	}

	const Player& getCurrentPlayer() const
	{
		return currentPlayer;
	}

	const MapInfo& getMapInfo() const
	{
		return mapInfo;
	}

	const std::vector<Player>& getPlayers() const
	{
		return players;
	}

	const BombInfo& getBombInfo() const
	{
		return bombInfo;
	}
};

#endif //GAMESTATE_H
