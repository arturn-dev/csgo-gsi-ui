#include "GSIPacketParser.h"

#include <plog/Init.h>
#include <plog/Log.h>

GSIPacketParser::GSIPacketParser()
{
	plog::init(plog::debug, &consoleAppender);
}

template<typename T>
void GSIPacketParser::setValueFromJson(T& destination, const nlohmann::json& json, const std::string& key)
{
	if (!json.contains(key))
	{
		LOG(plog::debug) << "Key '" << key << "' not found";
		return;
	}

	try
	{
		destination = json.value(key, destination);
	}
	catch (std::exception& e)
	{
		LOG(plog::error) << "Error getting a value under '" << key << "': " << e.what();
	}
}

template<typename T>
void GSIPacketParser::setMappedValueFromJson(T& destination, const nlohmann::json& json, const std::string& key)
{
	if (!json.contains(key))
	{
		LOG(plog::debug) << "Key '" << key << "' not found";
		return;
	}

	try
	{
		destination = getMapping<T>(json.at(key));
	}
	catch (std::exception& e)
	{
		LOG(plog::error) << "Error getting a mapped value under '" << key << "': " << e.what();
	}
}

std::vector<nlohmann::json> GSIPacketParser::getVectorFromJson(const nlohmann::json& json, const std::string& key)
{
	if (!json.contains(key))
	{
		LOG(plog::debug) << "Key '" << key << "' not found";
		return {};
	}

	try
	{
		const auto& jsonObject = json.at(key);
		if (!jsonObject.is_object())
		{
			throw std::logic_error("Value under the key '" + key + "' is not a json object.");
		}

		std::vector<nlohmann::json> vRet;
		for (const auto& entry: jsonObject.get<std::map<nlohmann::json, nlohmann::json>>())
		{
			vRet.push_back(entry.second);
		}

		return vRet;
	}
	catch (const std::exception& e)
	{
		LOG(plog::error) << "Error getting an object list under '" << key << "': " << e.what();
	}

	return {};
}

std::map<nlohmann::json, nlohmann::json>
GSIPacketParser::getMapFromJson(const nlohmann::json& json)
{
	try
	{
		if (!json.is_object())
		{
			// TODO: Find out how to get a key for which the value is not an object to include to the error msg.
			throw std::logic_error("Not a json object");
		}
		return json;
	}
	catch (const std::exception& e)
	{
		LOG(plog::error) << "Parser error: " << e.what();
	}

	return {};
}

template<typename T>
T GSIPacketParser::getMapping(const nlohmann::json& json)
{
	std::string jsonValue;
	if (json.is_string())
	{
		jsonValue = json.get<std::string>();
	}

	auto mapper = MapperTypeMap<T>::mapper;
	auto found = mapper.find(jsonValue);
	if (found == mapper.end())
	{
		return mapper.at({});
	} else
	{
		return mapper.at(jsonValue);
	}
}

template<>
GameState::Provider GSIPacketParser::getMapping(const nlohmann::json& json)
{
	GameState::Provider provider;
	setValueFromJson(provider.name, json, "name");
	setValueFromJson(provider.appId, json, "appid");
	setValueFromJson(provider.version, json, "version");
	setValueFromJson(provider.steamId, json, "steamid");
	setValueFromJson(provider.timestamp, json, "timestamp");

	return provider;
}

template<>
GameState::Vec3 GSIPacketParser::getMapping(const nlohmann::json& json)
{
	double values[3] = {std::numeric_limits<double>::min()};
	if (!json.is_string())
	{
		return {values[0], values[1], values[2]};
	}

	auto valuesStr = json.get<std::string>();
	auto valueEndPos = valuesStr.find(',');
	if (valueEndPos == std::string::npos)
	{
		return {values[0], values[1], values[2]};
	}

	try
	{
		size_t valueBeginPos = 0;
		for (int i = 0; i < 3; i++)
		{
			values[i] = std::stod(valuesStr.substr(valueBeginPos, valueEndPos - valueBeginPos));
			valueBeginPos = valueEndPos + 2;
			if (i == 1)
			{
				// Next value should be last, so no comma to find
				valueEndPos = valuesStr.size();
			} else
			{
				valueEndPos = valuesStr.find(',', valueBeginPos);
			}
		}
	}
	catch (std::exception& e)
	{
		LOG(plog::debug) << "Couldn't parse vec3: " << e.what();
	}

	return {values[0], values[1], values[2]};
}

template<>
GameState::MapInfo GSIPacketParser::getMapping(const nlohmann::json& json)
{
	GameState::MapInfo mapInfo;
	std::vector<GameState::MapInfo::RoundWinCause> roundWins;

	for (auto&& roundWin: getVectorFromJson(json, "round_wins"))
		roundWins.push_back(getMapping<GameState::MapInfo::RoundWinCause>(roundWin));

	setMappedValueFromJson(mapInfo.mode, json, "mode");
	setValueFromJson(mapInfo.name, json, "name");
	setMappedValueFromJson(mapInfo.phase, json, "phase");
	setValueFromJson(mapInfo.roundNo, json, "round");
	setMappedValueFromJson(mapInfo.ctSideStats, json, "team_ct");
	setMappedValueFromJson(mapInfo.tSideStats, json, "team_t");
	setValueFromJson(mapInfo.numberOfMatchesToWinSeries, json, "num_matches_to_win_series");
	setValueFromJson(mapInfo.currentSpectatorsCount, json, "current_spectators");
	setValueFromJson(mapInfo.souvenirsTotal, json, "souvenirs_total");
	mapInfo.roundWins = std::move(roundWins);

	return mapInfo;
}

template<>
GameState::MapInfo::SideStats GSIPacketParser::getMapping(const nlohmann::json& json)
{
	GameState::MapInfo::SideStats sideStats{GameState::SIDE_UNKNOWN};
	setValueFromJson(sideStats.score, json, "score");
	setValueFromJson(sideStats.consecutiveRoundLosses, json, "consecutive_round_losses");
	setValueFromJson(sideStats.timeoutsRemaining, json, "timeouts_remaining");
	setValueFromJson(sideStats.matchesWonThisSeries, json, "matches_won_this_series");

	return sideStats;
}

template<>
GameState::Weapon GSIPacketParser::getMapping(const nlohmann::json& json)
{
	GameState::Weapon weapon;
	setValueFromJson(weapon.name, json, "name");
	setValueFromJson(weapon.paintKit, json, "paintkit");
	setMappedValueFromJson(weapon.type, json, "type");
	setMappedValueFromJson(weapon.state, json, "state");
	setValueFromJson(weapon.ammoClip, json, "ammo_clip");
	setValueFromJson(weapon.ammoClipMax, json, "ammo_clip_max");
	setValueFromJson(weapon.ammoReserve, json, "ammo_reserve");

	return weapon;
}

template<>
GameState::BombInfo GSIPacketParser::getMapping(const nlohmann::json& json)
{
	GameState::BombInfo bombInfo;
	setMappedValueFromJson(bombInfo.bombState, json, "state");
	setMappedValueFromJson(bombInfo.position, json, "position");
	setMappedValueFromJson(bombInfo.countdown, json, "countdown");

	return bombInfo;
}

template<>
GameState::Player GSIPacketParser::getMapping(const nlohmann::json& json)
{
	GameState::Player player;
	if (!json.is_object())
	{
		return player;
	}

	setValueFromJson(player.name, json, "name");
	setValueFromJson(player.observerSlot, json, "observer_slot");
	setMappedValueFromJson(player.team, json, "team");
	setMappedValueFromJson(player.roundState, json, "state");
	setMappedValueFromJson(player.matchStats, json, "match_stats");

	for (auto&& weapon: getVectorFromJson(json, "weapons"))
		player.weapons.push_back(getMapping<GameState::Weapon>(weapon));

	setValueFromJson(player.specTarget, json, "spectarget");
	setMappedValueFromJson(player.position, json, "position");
	setMappedValueFromJson(player.forward, json, "forward");

	return player;
}

template<>
GameState::PlayerList GSIPacketParser::getMapping(const nlohmann::json& json)
{
	GameState::PlayerList players;

	if (!json.is_object())
	{
		PLOG(plog::error) << "No object under 'allplayers' key";
		return players;
	}

	for (auto it = json.begin(); it != json.end(); it++)
	{
		auto player = getMapping<GameState::Player>(it.value());
		player.steamId = it.key();
		players.push_back(std::move(player));
	}

	return players;
}

template<>
GameState::Player::RoundState GSIPacketParser::getMapping(const nlohmann::json& json)
{
	GameState::Player::RoundState roundState{};

	setValueFromJson(roundState.health, json, "health");
	setValueFromJson(roundState.armor, json, "armor");
	setValueFromJson(roundState.helmet, json, "helmet");
	setValueFromJson(roundState.flashed, json, "flashed");
	setValueFromJson(roundState.smoked, json, "smoked");
	setValueFromJson(roundState.burning, json, "burning");
	setValueFromJson(roundState.money, json, "money");
	setValueFromJson(roundState.kills, json, "round_kills");
	setValueFromJson(roundState.killHeadshots, json, "round_killhs");
	setValueFromJson(roundState.totalDamage, json, "round_totaldmg");
	setValueFromJson(roundState.equipmentValue, json, "equip_value");

	return roundState;
}

template<>
GameState::Player::MatchStats GSIPacketParser::getMapping(const nlohmann::json& json)
{
	GameState::Player::MatchStats matchStats{};

	setValueFromJson(matchStats.kills, json, "kills");
	setValueFromJson(matchStats.assists, json, "assists");
	setValueFromJson(matchStats.deaths, json, "deaths");
	setValueFromJson(matchStats.mvps, json, "mvps");
	setValueFromJson(matchStats.score, json, "score");

	return matchStats;
}

template<>
double GSIPacketParser::getMapping(const nlohmann::json& json)
{
	return std::stod(json.get<std::string>());
}

GameState GSIPacketParser::parse(nlohmann::json json)
{
	GameState::Provider provider;
	GameState::MapInfo mapInfo;
	GameState::BombInfo bombInfo;
	GameState::PlayerList players;

	setMappedValueFromJson(provider, json, "provider");
	setMappedValueFromJson(mapInfo, json, "map");
	setMappedValueFromJson(bombInfo, json, "bomb");
	setMappedValueFromJson(players, json, "allplayers");

	return {provider, mapInfo, players, bombInfo};
}

// Mappings of strings from json to enum types (GameState class)

template<>
const std::map<std::string, GameState::Side> GSIPacketParser::MapperTypeMap<GameState::Side>::mapper = {
		{"CT", GameState::CT_SIDE},
		{"T",  GameState::T_SIDE},
		{{},   GameState::SIDE_UNKNOWN}
};

template<>
const std::map<std::string, GameState::Weapon::Type> GSIPacketParser::MapperTypeMap<GameState::Weapon::Type>::mapper = {
		{"Knife",       GameState::Weapon::KNIFE},
		{"Pistol",      GameState::Weapon::PISTOL},
		{"Rifle",       GameState::Weapon::RIFLE},
		{"Grenade",     GameState::Weapon::GRENADE},
		{"SniperRifle", GameState::Weapon::SNIPER_RIFLE},
		{"C4",          GameState::Weapon::C4},
		{{},            GameState::Weapon::WEAPON_TYPE_UNKNOWN}
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
		{"carried", GameState::BombInfo::CARRIED},
		{{},        GameState::BombInfo::BOMB_STATE_UNKNOWN},
};