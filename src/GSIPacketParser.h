#ifndef GSIPACKETPARSER_H
#define GSIPACKETPARSER_H

#include <map>
#include <any>

#include "DisableMsvcWarnings.h"
#include <json.hpp>
#include "EnableMsvcWarnings.h"
#include "GameState.h"

template<typename T>
struct MapperTypeMap
{
	static const std::map<std::string, T> mapper;
};

template<>
const std::map<std::string, GameState::Side> MapperTypeMap<GameState::Side>::mapper = {
		{"CT", GameState::CT_SIDE},
		{"T",  GameState::T_SIDE},
		{{},   GameState::SIDE_UNKNOWN}
};

template<>
const std::map<std::string, GameState::Weapon::Type> MapperTypeMap<GameState::Weapon::Type>::mapper = {
		{"Knife",   GameState::Weapon::KNIFE},
		{"Pistol",  GameState::Weapon::PISTOL},
		{"Rifle",   GameState::Weapon::RIFLE},
		{"Grenade", GameState::Weapon::GRENADE},
		{{},        GameState::Weapon::WEAPON_TYPE_UNKNOWN}
};

template<>
const std::map<std::string, GameState::Weapon::State> MapperTypeMap<GameState::Weapon::State>::mapper = {
		{"holstered", GameState::Weapon::HOLSTERED},
		{"active",    GameState::Weapon::ACTIVE},
		{{},          GameState::Weapon::WEAPON_STATE_UNKNOWN},
};

template<>
const std::map<std::string, GameState::GamePhase> MapperTypeMap<GameState::GamePhase>::mapper = {
		{"live", GameState::GamePhase::LIVE},
		{{},     GameState::GamePhase::GAME_PHASE_UNKNOWN},
};

template<>
const std::map<std::string, GameState::MapInfo::Mode> MapperTypeMap<GameState::MapInfo::Mode>::mapper = {
		{"competitive", GameState::MapInfo::COMPETITIVE},
		{{},            GameState::MapInfo::MAP_MODE_UNKNOWN},
};

template<>
const std::map<std::string, GameState::MapInfo::RoundWinCause> MapperTypeMap<GameState::MapInfo::RoundWinCause>::mapper = {
		{"ct_win_elimination", GameState::MapInfo::CT_WIN_ELIMINATION},
		{"t_win_elimination",  GameState::MapInfo::T_WIN_ELIMINATION},
		{"ct_win_defuse",      GameState::MapInfo::CT_WIN_DEFUSE},
		{"t_win_bomb",         GameState::MapInfo::T_WIN_BOMB},
		{{},                   GameState::MapInfo::MAP_ROUND_WIN_CAUSE_UNKNOWN}
};

template<>
const std::map<std::string, GameState::BombInfo::BombState> MapperTypeMap<GameState::BombInfo::BombState>::mapper = {
		{"planted", GameState::BombInfo::PLANTED},
		{{},        GameState::BombInfo::BOMB_STATE_UNKNOWN},
};

class GSIPacketParser
{
	template<typename T>
	T getMapping(const nlohmann::json& json);

	GameState::MapInfo::SideStats getMapping(const nlohmann::json& json, GameState::Side side);

	GameState::Player parseToPlayer(const nlohmann::json& json, const std::string& steamId);

	void parsePlayerState(const nlohmann::json& json, GameState::Player& player);

	void parsePlayerStats(const nlohmann::json& json, GameState::Player& player);

	int getOptionalInt(const nlohmann::json& json, const std::string& key);

public:
	GameState parse(nlohmann::json json);
};

#endif //GSIPACKETPARSER_H
