#include "GSIPacketParser.h"

template<typename T>
T GSIPacketParser::getMapping(const nlohmann::json& json)
{
	auto jsonValue = json.get<std::string>();
	auto mapper = MapperTypeMap<T>::mapper;
	auto found = mapper.find(jsonValue);
	if (found == mapper.end())
		return mapper.at({});
	else
		return mapper.at(jsonValue);
}

template<>
GameState::Provider GSIPacketParser::getMapping(const nlohmann::json& json)
{
	GameState::Provider provider;
	provider.name = json.at("name");
	provider.appId = json.at("appid");
	provider.version = json.at("version");
	provider.steamId = json.at("steamid");
	provider.timestamp = json.at("timestamp");

	return provider;
}

template<>
GameState::Vec3 GSIPacketParser::getMapping(const nlohmann::json& json)
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

template<>
GameState::MapInfo GSIPacketParser::getMapping(const nlohmann::json& json)
{
	GameState::MapInfo mapInfo;
	std::vector<GameState::MapInfo::RoundWinCause> roundWins;

	for (auto&& roundWin: json.at("round_wins"))
		roundWins.push_back(getMapping<GameState::MapInfo::RoundWinCause>(roundWin));

	mapInfo.mode = getMapping<GameState::MapInfo::Mode>(json.at("mode"));
	mapInfo.name = json.at("name");
	mapInfo.phase = json.at("phase");
	mapInfo.roundNo = json.at("round");
	mapInfo.ctSideStats = getMapping(json.at("team_ct"), GameState::CT_SIDE);
	mapInfo.tSideStats = getMapping(json.at("team_t"), GameState::T_SIDE);
	mapInfo.numberOfMatchesToWinSeries = json.at("num_matches_to_win_series");
	mapInfo.currentSpectatorsCount = json.at("current_spectators");
	mapInfo.souvenirsTotal = json.at("souvenirs_total");
	mapInfo.roundWins = std::move(roundWins);

	return mapInfo;
}

GameState::MapInfo::SideStats GSIPacketParser::getMapping(const nlohmann::json& json, GameState::Side side)
{
	GameState::MapInfo::SideStats sideStats;
	sideStats.side = side;
	sideStats.score = json.at("score");
	sideStats.consecutiveRoundLosses = json.at("consecutive_round_losses");
	sideStats.timeoutsRemaining = json.at("timeouts_remaining");
	sideStats.matchesWonThisSeries = json.at("matches_won_this_series");

	return sideStats;
}

template<>
GameState::Weapon GSIPacketParser::getMapping(const nlohmann::json& json)
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

template<>
GameState::BombInfo GSIPacketParser::getMapping(const nlohmann::json& json)
{
	GameState::BombInfo bombInfo;
	bombInfo.bombState = getMapping<GameState::BombInfo::BombState>(json.at("state"));
	bombInfo.position = getMapping<GameState::Vec3>(json.at("position"));
	bombInfo.countdown = std::stod(json.at("countdown").get<std::string>());

	return bombInfo;
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
	player.position = getMapping<GameState::Vec3>(json.at("position"));
	player.forward = getMapping<GameState::Vec3>(json.at("forward"));

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

int GSIPacketParser::getOptionalInt(const nlohmann::json& json, const std::string& key)
{
	auto found = json.find(key);
	if (found == json.end())
		return -1;
	else
		return found.value();
}

GameState GSIPacketParser::parse(nlohmann::json json)
{
	auto provider = getMapping<GameState::Provider>(json.at("provider"));
	auto mapInfo = getMapping<GameState::MapInfo>(json.at("map"));
	auto bombInfo = getMapping<GameState::BombInfo>(json.at("bomb"));
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

template<>
const std::map<std::string, GameState::Side> GSIPacketParser::MapperTypeMap<GameState::Side>::mapper = {
		{"CT", GameState::CT_SIDE},
		{"T",  GameState::T_SIDE},
		{{},   GameState::SIDE_UNKNOWN}
};

template<>
const std::map<std::string, GameState::Weapon::Type> GSIPacketParser::MapperTypeMap<GameState::Weapon::Type>::mapper = {
		{"Knife",   GameState::Weapon::KNIFE},
		{"Pistol",  GameState::Weapon::PISTOL},
		{"Rifle",   GameState::Weapon::RIFLE},
		{"Grenade", GameState::Weapon::GRENADE},
		{{},        GameState::Weapon::WEAPON_TYPE_UNKNOWN}
};

template<>
const std::map<std::string, GameState::Weapon::State> GSIPacketParser::MapperTypeMap<GameState::Weapon::State>::mapper = {
		{"holstered", GameState::Weapon::HOLSTERED},
		{"active",    GameState::Weapon::ACTIVE},
		{{},          GameState::Weapon::WEAPON_STATE_UNKNOWN},
};

template<>
const std::map<std::string, GameState::GamePhase> GSIPacketParser::MapperTypeMap<GameState::GamePhase>::mapper = {
		{"live", GameState::GamePhase::LIVE},
		{{},     GameState::GamePhase::GAME_PHASE_UNKNOWN},
};

template<>
const std::map<std::string, GameState::MapInfo::Mode> GSIPacketParser::MapperTypeMap<GameState::MapInfo::Mode>::mapper = {
		{"competitive", GameState::MapInfo::COMPETITIVE},
		{{},            GameState::MapInfo::MAP_MODE_UNKNOWN},
};

template<>
const std::map<std::string, GameState::MapInfo::RoundWinCause> GSIPacketParser::MapperTypeMap<GameState::MapInfo::RoundWinCause>::mapper = {
		{"ct_win_elimination", GameState::MapInfo::CT_WIN_ELIMINATION},
		{"t_win_elimination",  GameState::MapInfo::T_WIN_ELIMINATION},
		{"ct_win_defuse",      GameState::MapInfo::CT_WIN_DEFUSE},
		{"t_win_bomb",         GameState::MapInfo::T_WIN_BOMB},
		{{},                   GameState::MapInfo::MAP_ROUND_WIN_CAUSE_UNKNOWN}
};

template<>
const std::map<std::string, GameState::BombInfo::BombState> GSIPacketParser::MapperTypeMap<GameState::BombInfo::BombState>::mapper = {
		{"planted", GameState::BombInfo::PLANTED},
		{{},        GameState::BombInfo::BOMB_STATE_UNKNOWN},
};