#include <fstream>
#include "gtest/gtest.h"
#include "gtest/gtest-printers.h"
#include "../../src/GSIPacketParser.h"

void assertVec3(const GameState::Vec3& lhs, const GameState::Vec3& rhs)
{
	ASSERT_EQ(lhs.x, rhs.x);
	ASSERT_EQ(lhs.y, rhs.y);
	ASSERT_EQ(lhs.z, rhs.z);
}

void assertBombInfo(const GameState::BombInfo& lhs, const GameState::BombInfo& rhs)
{
	ASSERT_EQ(lhs.bombState, rhs.bombState);
	ASSERT_NO_FATAL_FAILURE(assertVec3(lhs.position, rhs.position));
	ASSERT_EQ(lhs.countdown, rhs.countdown);
}

void assertSideStats(const GameState::MapInfo::SideStats& lhs, const GameState::MapInfo::SideStats& rhs)
{
	ASSERT_EQ(lhs.side, rhs.side);
	ASSERT_EQ(lhs.score, rhs.score);
	ASSERT_EQ(lhs.consecutiveRoundLosses, rhs.consecutiveRoundLosses);
	ASSERT_EQ(lhs.timeoutsRemaining, rhs.timeoutsRemaining);
	ASSERT_EQ(lhs.matchesWonThisSeries, rhs.matchesWonThisSeries);
}

void assertMapInfo(const GameState::MapInfo& lhs, const GameState::MapInfo& rhs)
{
	ASSERT_EQ(lhs.mode, rhs.mode);
	ASSERT_EQ(lhs.name, rhs.name);
	ASSERT_EQ(lhs.phase, rhs.phase);
	ASSERT_EQ(lhs.roundNo, rhs.roundNo);
	ASSERT_NO_FATAL_FAILURE(assertSideStats(lhs.ctSideStats, rhs.ctSideStats));
	ASSERT_NO_FATAL_FAILURE(assertSideStats(lhs.tSideStats, rhs.tSideStats));
	ASSERT_EQ(lhs.numberOfMatchesToWinSeries, rhs.numberOfMatchesToWinSeries);
	ASSERT_EQ(lhs.currentSpectatorsCount, rhs.currentSpectatorsCount);
	ASSERT_EQ(lhs.souvenirsTotal, rhs.souvenirsTotal);
	ASSERT_EQ(lhs.roundWins, rhs.roundWins);
}

void assertPlayerRoundState(const GameState::Player::RoundState& lhs, const GameState::Player::RoundState& rhs)
{
	ASSERT_EQ(lhs.health, rhs.health);
	ASSERT_EQ(lhs.armor, rhs.armor);
	ASSERT_EQ(lhs.helmet, rhs.helmet);
	ASSERT_EQ(lhs.flashed, rhs.flashed);
	ASSERT_EQ(lhs.smoked, rhs.smoked);
	ASSERT_EQ(lhs.burning, rhs.burning);
	ASSERT_EQ(lhs.money, rhs.money);
	ASSERT_EQ(lhs.kills, rhs.kills);
	ASSERT_EQ(lhs.killHeadshots, rhs.killHeadshots);
	ASSERT_EQ(lhs.totalDamage, rhs.totalDamage);
	ASSERT_EQ(lhs.equipmentValue, rhs.equipmentValue);
}

void assertMatchStats(const GameState::Player::MatchStats& lhs, const GameState::Player::MatchStats& rhs)
{
	ASSERT_EQ(lhs.kills, rhs.kills);
	ASSERT_EQ(lhs.assists, rhs.assists);
	ASSERT_EQ(lhs.deaths, rhs.deaths);
	ASSERT_EQ(lhs.mvps, rhs.mvps);
	ASSERT_EQ(lhs.score, rhs.score);
}

void assertWeapon(const GameState::Weapon& lhs, const GameState::Weapon& rhs)
{
	ASSERT_EQ(lhs.name, rhs.name);
	ASSERT_EQ(lhs.paintKit, rhs.paintKit);
	ASSERT_EQ(lhs.type, rhs.type);
	ASSERT_EQ(lhs.ammoClip, rhs.ammoClip);
	ASSERT_EQ(lhs.ammoClipMax, rhs.ammoClipMax);
	ASSERT_EQ(lhs.ammoReserve, rhs.ammoReserve);
	ASSERT_EQ(lhs.state, rhs.state);
}

void assertPlayer(const GameState::Player& lhs, const GameState::Player& rhs)
{
	ASSERT_EQ(lhs.steamId, rhs.steamId);
	ASSERT_EQ(lhs.name, rhs.name);
	ASSERT_EQ(lhs.observerSlot, rhs.observerSlot);
	ASSERT_EQ(lhs.team, rhs.team);
	ASSERT_NO_FATAL_FAILURE(assertPlayerRoundState(lhs.roundState, rhs.roundState));
	ASSERT_NO_FATAL_FAILURE(assertMatchStats(lhs.matchStats, rhs.matchStats));
	for (int i = 0; i < lhs.weapons.size(); ++i)
	{
		ASSERT_NO_FATAL_FAILURE(assertWeapon(lhs.weapons.at(i), rhs.weapons.at(i))) << "i was " + std::to_string(i);
	}
	ASSERT_EQ(lhs.specTarget, rhs.specTarget);
	ASSERT_NO_FATAL_FAILURE(assertVec3(lhs.position, rhs.position));
	ASSERT_NO_FATAL_FAILURE(assertVec3(lhs.forward, rhs.forward));
}

void assertProvider(const GameState::Provider& lhs, const GameState::Provider& rhs)
{
	ASSERT_EQ(lhs.name, rhs.name);
	ASSERT_EQ(lhs.appId, rhs.appId);
	ASSERT_EQ(lhs.version, rhs.version);
	ASSERT_EQ(lhs.steamId, rhs.steamId);
	ASSERT_EQ(lhs.timestamp, rhs.timestamp);
}

GameState::Player
createPlayer(const std::string& steamId, const std::string& name, int obsSlot, GameState::Side side,
			 GameState::Player::MatchStats matchStats, GameState::Vec3 pos, GameState::Vec3 fwd,
			 GameState::Player::RoundState roundState, const std::vector<GameState::Weapon>& weapons)
{
	GameState::Player player;
	player.steamId = steamId;
	player.name = name;
	player.observerSlot = obsSlot;
	player.team = side;
	player.matchStats = matchStats;
	player.position = pos;
	player.forward = fwd;
	player.roundState = roundState;
	player.weapons = weapons;

	return player;
}

GameState::Weapon createWeapon(const std::string& name, const std::string& paintkit, GameState::Weapon::Type type,
							   GameState::Weapon::State state, int ammoClip = -1, int ammoClipMax = -1,
							   int ammoReserve = -1)
{
	GameState::Weapon weapon;
	weapon.name = name;
	weapon.paintKit = paintkit;
	weapon.type = type;
	weapon.ammoClip = ammoClip;
	weapon.ammoClipMax = ammoClipMax;
	weapon.ammoReserve = ammoReserve;
	weapon.state = state;

	return weapon;
}

GameState::Weapon createGrenade(const std::string& name, GameState::Weapon::State state, int ammoReserve = 1)
{
	return createWeapon(name, "default", GameState::Weapon::GRENADE, state, -1, -1, ammoReserve);
}

void parseFileToGameState(const std::string& filePath, GameState& gameState)
{
	std::ifstream ifs(filePath);
	if (!ifs.good())
		FAIL() << "Couldn't open the file:  " + filePath;

	std::stringstream ss;
	ss << ifs.rdbuf();
	auto json = nlohmann::json::parse(ss.str());
	GSIPacketParser parser;
	gameState = parser.parse(json);
}

TEST(GSIPacketParser, ShouldCorrectlyParsePlayers)
{
	GameState::PlayerList playerList;
	playerList.push_back(createPlayer(
			"0", "Player 0", 0, GameState::T_SIDE, {9, 0, 8, 0, 19},
			{807.02, 2491.80, 138.57},
			{-0.93, 0.36, 0.05}, {100, 99, true, 0, -1, 0, 900, 0, 0, 0, 4100},
			{
					createWeapon("weapon_knife_karambit", "aq_damascus",
								 GameState::Weapon::KNIFE, GameState::Weapon::HOLSTERED),
					createWeapon("weapon_glock", "cu_glock-liquescent",
								 GameState::Weapon::PISTOL, GameState::Weapon::HOLSTERED, 20,
								 20, 120),
					createWeapon("weapon_ak47", "aq_ak47_cartel", GameState::Weapon::RIFLE,
								 GameState::Weapon::ACTIVE, 30, 30, 29),
					createWeapon("weapon_c4", "default", GameState::Weapon::C4,
								 GameState::Weapon::HOLSTERED)
			}));
	playerList.push_back(createPlayer(
			"1", "Player 1", 1, GameState::CT_SIDE, {8, 6, 4, 0, 22},
			{2349.31, -349.82, 88.63},
			{-0.63, 0.78, 0.05}, {73, 96, true, 0, -1, 0, 5350, 1, 1, 100, 5700},
			{
					createWeapon("weapon_knife", "default", GameState::Weapon::KNIFE,
								 GameState::Weapon::HOLSTERED),
					createWeapon("weapon_usp_silencer", "default", GameState::Weapon::PISTOL,
								 GameState::Weapon::HOLSTERED, 12, 12, 24),
					createWeapon("weapon_ak47", "aq_ak47_cartel", GameState::Weapon::RIFLE,
								 GameState::Weapon::ACTIVE, 21, 30, 90),
					createGrenade("weapon_hegrenade", GameState::Weapon::HOLSTERED),
					createGrenade("weapon_flashbang", GameState::Weapon::HOLSTERED),
			}));
	playerList.push_back(createPlayer(
			"2", "Player 2", 2, GameState::CT_SIDE, {8, 0, 4, 2, 16},
			{1816.31, 1401.86, 160.03},
			{-0.16, 0.99, -0.07}, {100, 94, true, 0, -1, 0, 7400, 0, 0, 0, 7850},
			{
					createWeapon("weapon_knife_gut", "aq_damascus", GameState::Weapon::KNIFE,
								 GameState::Weapon::ACTIVE),
					createWeapon("weapon_deagle", "default", GameState::Weapon::PISTOL,
								 GameState::Weapon::HOLSTERED, 7, 7, 35),
					createWeapon("weapon_awp", "cu_awp_vein", GameState::Weapon::SNIPER_RIFLE,
								 GameState::Weapon::HOLSTERED, 9, 10, 30)
			}));
	playerList.push_back(createPlayer(
			"3", "Player 3", 3, GameState::CT_SIDE, {13, 1, 2, 3, 29},
			{473.85, 2574.22, 160.03},
			{0.92, -0.39, -0.06}, {100, 100, true, 0, -1, 0, 5800, 1, 1, 66, 5700},
			{
					createWeapon("weapon_knife", "default", GameState::Weapon::KNIFE,
								 GameState::Weapon::HOLSTERED),
					createWeapon("weapon_p250", "cu_xray_p250", GameState::Weapon::PISTOL,
								 GameState::Weapon::HOLSTERED, 13, 13, 26),
					createWeapon("weapon_ak47", "cu_ak47_asiimov", GameState::Weapon::RIFLE,
								 GameState::Weapon::ACTIVE, 24, 30, 90)
			}));
	playerList.push_back(createPlayer(
			"4", "Player 4", 4, GameState::CT_SIDE, {11, 1, 6, 2, 23},
			{1463.19, -96.67, 130.03},
			{0.98, -0.20, -0.07}, {0, 0, false, 0, -1, 0, 2950, 1, 1, 100, 6200}, {}));
	playerList.push_back(createPlayer(
			"5", "Player 5", 5, GameState::CT_SIDE, {5, 2, 7, 1, 12},
			{471.38, 2120.02, 135.12},
			{0.96, -0.26, -0.07}, {0, 0, false, 0, -1, 0, 2700, 1, 0, 134, 5000}, {}));
	playerList.push_back(createPlayer(
			"6", "Player 6", 6, GameState::T_SIDE, {5, 3, 9, 1, 15},
			{2014.46, 100.25, 132.93}, {-0.89, 0.45, 0.04},
			{0, 0, false, 0, -1, 0, 550, 1, 1, 127, 3900}, {}));
	playerList.push_back(createPlayer(
			"7", "Player 7", 7, GameState::T_SIDE, {1, 0, 9, 0, 4},
			{1254.51, 328.01, 128.03},
			{-0.89, 0.45, 0.04}, {0, 0, false, 0, -1, 0, 100, 0, 0, 0, 3900}, {}));
	playerList.push_back(createPlayer(
			"8", "Player 8", 8, GameState::T_SIDE, {2, 2, 10, 0, 6},
			{50.68, 874.80, 70.01},
			{-0.89, 0.45, 0.04}, {0, 0, false, 0, -1, 0, 0, 0, 0, 51, 3600}, {}));
	playerList.push_back(createPlayer(
			"9", "Player 9", 9, GameState::T_SIDE, {6, 0, 9, 0, 12},
			{696.08, 2773.96, 132.77},
			{0.44, -0.90, 0.02}, {0, 0, false, 255, -1, 0, 800, 1, 1, 49, 4600}, {}));

	GameState gameState;
	ASSERT_NO_FATAL_FAILURE(parseFileToGameState("../test/data/players.json", gameState));

	for (int i = 0; i < playerList.size(); ++i)
	{
		auto playerExpected = playerList.at(i);
		auto playerActual = gameState.getPlayers().at(i);
		ASSERT_NO_FATAL_FAILURE(assertPlayer(playerExpected, playerActual)) << "i was " + std::to_string(i);
	}
}

TEST(GSIPacketParser, ShouldCorrectlyParseMapInfo)
{
	GameState::MapInfo mapInfo;
	mapInfo.mode = GameState::MapInfo::COMPETITIVE;
	mapInfo.name = "de_inferno";
	mapInfo.phase = GameState::LIVE;
	mapInfo.roundNo = 9;
	mapInfo.numberOfMatchesToWinSeries = 0;
	mapInfo.currentSpectatorsCount = 1;
	mapInfo.souvenirsTotal = 0;

	mapInfo.ctSideStats.score = 8;
	mapInfo.ctSideStats.consecutiveRoundLosses = 0;
	mapInfo.ctSideStats.timeoutsRemaining = 1;
	mapInfo.ctSideStats.matchesWonThisSeries = 0;

	mapInfo.tSideStats.score = 1;
	mapInfo.tSideStats.consecutiveRoundLosses = 6;
	mapInfo.tSideStats.timeoutsRemaining = 1;
	mapInfo.tSideStats.matchesWonThisSeries = 0;

	using RoundWinCause = GameState::MapInfo::RoundWinCause;
	mapInfo.roundWins = {
			RoundWinCause::CT_WIN_DEFUSE,
			RoundWinCause::CT_WIN_ELIMINATION,
			RoundWinCause::CT_WIN_ELIMINATION,
			RoundWinCause::CT_WIN_ELIMINATION,
			RoundWinCause::CT_WIN_ELIMINATION,
			RoundWinCause::T_WIN_ELIMINATION,
			RoundWinCause::CT_WIN_ELIMINATION,
			RoundWinCause::CT_WIN_ELIMINATION,
			RoundWinCause::CT_WIN_ELIMINATION,
	};

	GameState gameState;
	ASSERT_NO_FATAL_FAILURE(parseFileToGameState("../test/data/map.json", gameState));

	ASSERT_NO_FATAL_FAILURE(assertMapInfo(mapInfo, gameState.getMapInfo()));
}

TEST(GSIPacketParser, ShouldCorrectlyParseProvider)
{
	GameState::Provider provider;
	provider.name = "Counter-Strike: Global Offensive",
			provider.appId = 730;
	provider.version = 13831;
	provider.steamId = "9999";
	provider.timestamp = 1652817929;

	GameState gameState;
	ASSERT_NO_FATAL_FAILURE(parseFileToGameState("../test/data/provider.json", gameState));

	ASSERT_NO_FATAL_FAILURE(assertProvider(provider, gameState.getProvider()));
}

TEST(GSIPacketParser, ShouldCorrectlyParseBombInfo)
{
	GameState::BombInfo bombInfo;
	bombInfo.bombState = GameState::BombInfo::CARRIED;
	bombInfo.position = {807.02, 2491.80, 138.57};
	bombInfo.countdown = INFINITY;

	GameState gameState;
	ASSERT_NO_FATAL_FAILURE(parseFileToGameState("../test/data/bomb_info.json", gameState));

	ASSERT_NO_FATAL_FAILURE(assertBombInfo(bombInfo, gameState.getBombInfo()));
}