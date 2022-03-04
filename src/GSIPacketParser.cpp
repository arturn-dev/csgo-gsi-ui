#include "GSIPacketParser.h"

GameState GSIPacketParser::parse(nlohmann::json json)
{
	auto provider = parseToProvider(json.at("provider"));
	auto mapInfo = parseToMapInfo(json.at("map"));
	auto bombInfo = parseToBombInfo(json.at("bomb"));
	auto currentPlayerSteamId = json.at("player").at("steamid").get<std::string>();

	std::list<GameState::Player> players;
	std::list<GameState::Player>::iterator currentPlayerIt;
	for (auto&& p: json.at("allplayers").items())
	{
		players.push_back(parseToPlayer(p.value(), p.key()));
		if (p.key() == currentPlayerSteamId)
		{
			currentPlayerIt = std::prev(players.end(), 1);
		}
	}

	return {provider, *currentPlayerIt, mapInfo, players, bombInfo};
}

GameState::Provider GSIPacketParser::parseToProvider(const nlohmann::json& json)
{
	GameState::Provider provider;
	provider.name = json.at("name");
	provider.appId = json.at("appid");
	provider.version = json.at("version");
	provider.steamId = json.at("steamid");
	provider.timestamp = json.at("timestamp");

	return provider;
}

GameState::MapInfo GSIPacketParser::parseToMapInfo(const nlohmann::json& json)
{
	GameState::MapInfo mapInfo;
	std::vector<GameState::MapInfo::RoundWinCause> roundWins;

	for (auto&& roundWin: json.at("round_wins"))
		roundWins.push_back(getMapping<GameState::MapInfo::RoundWinCause>(roundWin));

	mapInfo.mode = getMapping<GameState::MapInfo::Mode>(json.at("mode"));
	mapInfo.name = json.at("name");
	mapInfo.phase = json.at("phase");
	mapInfo.roundNo = json.at("round");
	mapInfo.ctSideStats = parseToSideStats(json.at("team_ct"), GameState::CT_SIDE);
	mapInfo.tSideStats = parseToSideStats(json.at("team_t"), GameState::T_SIDE);
	mapInfo.numberOfMatchesToWinSeries = json.at("num_matches_to_win_series");
	mapInfo.currentSpectatorsCount = json.at("current_spectators");
	mapInfo.souvenirsTotal = json.at("souvenirs_total");
	mapInfo.roundWins = std::move(roundWins);

	return mapInfo;
}

GameState::MapInfo::SideStats GSIPacketParser::parseToSideStats(const nlohmann::json& json, GameState::Side side)
{
	GameState::MapInfo::SideStats sideStats;
	sideStats.side = side;
	sideStats.score = json.at("score");
	sideStats.consecutiveRoundLosses = json.at("consecutive_round_losses");
	sideStats.timeoutsRemaining = json.at("timeouts_remaining");
	sideStats.matchesWonThisSeries = json.at("matches_won_this_series");

	return sideStats;
}

GameState::Weapon GSIPacketParser::parseToWeapon(const nlohmann::json& json)
{
	GameState::Weapon weapon;
	weapon.name = json.at("name");
	weapon.paintKit = json.at("paintkit");
	weapon.type = getMapping<GameState::Weapon::Type>(json.at("type"));
	weapon.ammoClip = getOptionalInt(json, "ammo_clip");
	weapon.ammoClipMax = getOptionalInt(json, "ammo_clip_max");
	weapon.ammoReserve = getOptionalInt(json, "ammo_reserve");
	weapon.state = getMapping<GameState::Weapon::State>(json.at("state"));

	return weapon;
}

GameState::Player GSIPacketParser::parseToPlayer(const nlohmann::json& json, const std::string& steamId)
{
	GameState::Player player;
	player.steamId = steamId;
	player.name = json.at("name");
	player.observerSlot = json.at("observer_slot");
	player.team = getMapping<GameState::Side>(json.at("team"));
	parsePlayerState(json.at("state"), player);
	parsePlayerStats(json.at("match_stats"), player);

	for (auto&& weapon: json.at("weapons"))
		player.weapons.push_back(getMapping<GameState::Weapon>(weapon));

	player.specTarget = json.at("spectarget");
	player.position = parseToVec3(json.at("position"));
	player.forward = parseToVec3(json.at("forward"));

	return player;
}

void GSIPacketParser::parsePlayerState(const nlohmann::json& json, GameState::Player& player)
{
	player.roundState.health = json.at("health");
	player.roundState.armor = json.at("armor");
	player.roundState.helmet = json.at("helmet");
	player.roundState.flashed = json.at("flashed");
	player.roundState.smoked = json.at("smoked");
	player.roundState.burning = json.at("burning");
	player.roundState.money = json.at("money");
	player.roundState.kills = json.at("round_kills");
	player.roundState.killHeadshots = json.at("round_killhs");
	player.roundState.totalDamage = json.at("round_totaldmg");
	player.roundState.equipmentValue = json.at("equip_value");
}

void GSIPacketParser::parsePlayerStats(const nlohmann::json& json, GameState::Player& player)
{
	player.matchStats.kills = json.at("kills");
	player.matchStats.assists = json.at("assists");
	player.matchStats.deaths = json.at("deaths");
	player.matchStats.mvps = json.at("mvps");
	player.matchStats.score = json.at("score");
}

GameState::BombInfo GSIPacketParser::parseToBombInfo(const nlohmann::json& json)
{
	GameState::BombInfo bombInfo;
	bombInfo.bombState = getMapping<GameState::BombInfo::BombState>(json.at("state"));
	bombInfo.position = parseToVec3(json.at("position"));
	bombInfo.countdown = std::stod(json.at("countdown").get<std::string>());

	return bombInfo;
}

GameState::Vec3 GSIPacketParser::parseToVec3(const nlohmann::json& json)
{
	double values[3] = {0.0};
	auto valuesStr = json.get<std::string>();
	auto commaPos = valuesStr.find(',');
	int i = 0;
	while (commaPos != std::string::npos && i < 3)
	{
		values[i] = std::stod(valuesStr.substr(0, commaPos - 1));
		valuesStr = valuesStr.substr(commaPos + 1);
		commaPos = valuesStr.find(',');
		i++;
	}

	return {values[0], values[1], values[2]};
}

int GSIPacketParser::getOptionalInt(const nlohmann::json& json, const std::string& key)
{
	auto found = json.find(key);
	if (found == json.end())
		return -1;
	else
		return found.value();
}

template<>
GameState::Side GSIPacketParser::getMapping<GameState::Side>(const nlohmann::json& json)
{
	auto jsonValue = json.get<std::string>();
	auto found = teamMapping.find(jsonValue);
	if (found == teamMapping.end())
		return teamMapping.at({});
	else
		return teamMapping.at(jsonValue);
}

template<>
GameState::Weapon::Type GSIPacketParser::getMapping<GameState::Weapon::Type>(const nlohmann::json& json)
{
	auto jsonValue = json.get<std::string>();
	auto found = weaponTypeMapping.find(jsonValue);
	if (found == weaponTypeMapping.end())
		return weaponTypeMapping.at({});
	else
		return weaponTypeMapping.at(jsonValue);
}

template<>
GameState::Weapon::State GSIPacketParser::getMapping<GameState::Weapon::State>(const nlohmann::json& json)
{
	auto jsonValue = json.get<std::string>();
	auto found = weaponStateMapping.find(jsonValue);
	if (found == weaponStateMapping.end())
		return weaponStateMapping.at({});
	else
		return weaponStateMapping.at(jsonValue);
}

template<>
GameState::GamePhase GSIPacketParser::getMapping<GameState::GamePhase>(const nlohmann::json& json)
{
	auto jsonValue = json.get<std::string>();
	auto found = gamePhaseMapping.find(jsonValue);
	if (found == gamePhaseMapping.end())
		return gamePhaseMapping.at({});
	else
		return gamePhaseMapping.at(jsonValue);
}

template<>
GameState::MapInfo::Mode GSIPacketParser::getMapping<GameState::MapInfo::Mode>(const nlohmann::json& json)
{
	auto jsonValue = json.get<std::string>();
	auto found = mapModeMapping.find(jsonValue);
	if (found == mapModeMapping.end())
		return mapModeMapping.at({});
	else
		return mapModeMapping.at(jsonValue);
}

template<>
GameState::MapInfo::RoundWinCause
GSIPacketParser::getMapping<GameState::MapInfo::RoundWinCause>(const nlohmann::json& json)
{
	auto jsonValue = json.get<std::string>();
	auto found = roundWinCauseMapping.find(jsonValue);
	if (found == roundWinCauseMapping.end())
		return roundWinCauseMapping.at({});
	else
		return roundWinCauseMapping.at(jsonValue);
}

template<>
GameState::BombInfo::BombState GSIPacketParser::getMapping<GameState::BombInfo::BombState>(const nlohmann::json& json)
{
	auto jsonValue = json.get<std::string>();
	auto found = bombStateMapping.find(jsonValue);
	if (found == bombStateMapping.end())
		return bombStateMapping.at({});
	else
		return bombStateMapping.at(jsonValue);
}
