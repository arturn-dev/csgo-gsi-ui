#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <string>
#include <utility>
#include <vector>

class GameState
{
public:
	struct Provider
	{
		std::string name;
		unsigned long appId = 0;
		unsigned long version = 0;
		std::string steamId;
		unsigned long timestamp = 0;
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
			GRENADE,
			C4,
			SNIPER_RIFLE
		} type = WEAPON_TYPE_UNKNOWN;

		int ammoClip = -1;
		int ammoClipMax = -1;
		int ammoReserve = -1;

		enum State
		{
			WEAPON_STATE_UNKNOWN,
			HOLSTERED,
			ACTIVE
		} state = WEAPON_STATE_UNKNOWN;
	};

	struct Vec3
	{
		double x = 0.0, y = 0.0, z = 0.0;
	};

	struct Player
	{
		std::string steamId;
		std::string name;
		unsigned int observerSlot = std::numeric_limits<unsigned int>::max();
		Side team = SIDE_UNKNOWN;

		struct RoundState
		{
			int health = -1;
			int armor = -1;
			bool helmet = false;
			int flashed = -1;
			int smoked = -1; // Valid only for currently spectated player, otherwise: -1
			int burning = -1;
			int money = -1;
			int kills = std::numeric_limits<unsigned int>::min();
			int killHeadshots = -1;
			int totalDamage = -1;
			int equipmentValue = -1;
		} roundState;

		struct MatchStats
		{
			int kills = std::numeric_limits<unsigned int>::min();
			int assists = std::numeric_limits<unsigned int>::min();
			int deaths = -1;
			int mvps = -1;
			int score = std::numeric_limits<unsigned int>::min();
		} matchStats;

		std::vector<Weapon> weapons;
		std::string specTarget; // Valid only for currently spectated player, otherwise empty
		Vec3 position;
		Vec3 forward;
	};

	typedef std::vector<GameState::Player> PlayerList;

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
		} mode = MAP_MODE_UNKNOWN;

		std::string name;

		GamePhase phase = GAME_PHASE_UNKNOWN;

		int roundNo = -1;

		struct SideStats
		{
			SideStats() : side(SIDE_UNKNOWN)
			{}

			explicit SideStats(const Side side) : side(side)
			{}

			SideStats(const SideStats& other) = default;

			SideStats& operator=(const SideStats& other);

			const Side side;
			int score = -1;
			int consecutiveRoundLosses = -1;
			int timeoutsRemaining = -1;
			int matchesWonThisSeries = -1;
		} ctSideStats{CT_SIDE}, tSideStats{T_SIDE};

		int numberOfMatchesToWinSeries = -1;
		int currentSpectatorsCount = -1;
		int souvenirsTotal = -1;

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
			CARRIED,
			DROPPED,
			PLANTING,
			PLANTED,
			DEFUSING,
			DEFUSED
		} bombState = BOMB_STATE_UNKNOWN;
		Vec3 position;
		double countdown = -1.0;
	};

	//enum Phase

private:
	Provider provider;
	//const Player& currentPlayer; TODO: Is it really necessary? Maybe some other idea to indicate currently viewed player.
	MapInfo mapInfo;
	PlayerList players;
	BombInfo bombInfo;
	// TODO: grenades, round?, phase_countdown?

public:
	GameState() = default;

	GameState(Provider provider, MapInfo mapInfo,
			  PlayerList players, BombInfo bombInfo) : provider(std::move(provider)),
													   mapInfo(std::move(mapInfo)),
													   players(std::move(players)),
													   bombInfo(bombInfo)
	{}

	const Provider& getProvider() const
	{
		return provider;
	}

	const MapInfo& getMapInfo() const
	{
		return mapInfo;
	}

	const PlayerList& getPlayers() const
	{
		return players;
	}

	const BombInfo& getBombInfo() const
	{
		return bombInfo;
	}
};

#endif //GAMESTATE_H
