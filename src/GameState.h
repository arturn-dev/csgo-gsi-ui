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
			GRENADE,
			C4,
			SNIPER_RIFLE
		} type;

		// optional values (-1 if undefined)
		int ammoClip = -1;
		int ammoClipMax = -1;
		int ammoReserve = -1;

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

		struct RoundState
		{
			int health;
			int armor;
			bool helmet;
			int flashed;
			int smoked = -1; // Valid only for currently spectated player, otherwise: -1
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
		} mode;

		std::string name;

		GamePhase phase;

		int roundNo;

		struct SideStats
		{
			explicit SideStats(const Side side) : side(side)
			{}

			SideStats(const SideStats& other) = default;

			SideStats& operator=(const SideStats& other);

			const Side side;
			int score{};
			int consecutiveRoundLosses{};
			int timeoutsRemaining{};
			int matchesWonThisSeries{};
		} ctSideStats{CT_SIDE}, tSideStats{T_SIDE};

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
			PLANTED,
			CARRIED
		} bombState;
		Vec3 position;
		double countdown = INFINITY;
	};

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
