#ifndef GSIPACKETPARSER_H
#define GSIPACKETPARSER_H

#include <map>

#include "DisableMsvcWarnings.h"
#include <json.hpp>
#include "EnableMsvcWarnings.h"
#include "GameState.h"

class GSIPacketParser
{
	const std::map<std::string, GameState::Side> teamMapping = {
			{"CT", GameState::CT_SIDE},
			{"T",  GameState::T_SIDE},
			{{},   GameState::SIDE_UNKNOWN}
	};

	const std::map<std::string, GameState::Weapon::Type> weaponTypeMapping = {
			{"Knife",   GameState::Weapon::KNIFE},
			{"Pistol",  GameState::Weapon::PISTOL},
			{"Rifle",   GameState::Weapon::RIFLE},
			{"Grenade", GameState::Weapon::GRENADE},
			{{},        GameState::Weapon::WEAPON_TYPE_UNKNOWN}
	};

	const std::map<std::string, GameState::Weapon::State> weaponStateMapping = {
			{"holstered", GameState::Weapon::HOLSTERED},
			{"active",    GameState::Weapon::ACTIVE},
			{{},          GameState::Weapon::WEAPON_STATE_UNKNOWN},
	};

	const std::map<std::string, GameState::GamePhase> gamePhaseMapping = {
			{"live", GameState::GamePhase::LIVE},
			{{},     GameState::GamePhase::GAME_PHASE_UNKNOWN},
	};

	const std::map<std::string, GameState::MapInfo::Mode> mapModeMapping = {
			{"competitive", GameState::MapInfo::COMPETITIVE},
			{{},            GameState::MapInfo::MAP_MODE_UNKNOWN},
	};

	const std::map<std::string, GameState::MapInfo::RoundWinCause> roundWinCauseMapping = {
			{"ct_win_elimination", GameState::MapInfo::CT_WIN_ELIMINATION},
			{"t_win_elimination",  GameState::MapInfo::T_WIN_ELIMINATION},
			{"ct_win_defuse",      GameState::MapInfo::CT_WIN_DEFUSE},
			{"t_win_bomb",         GameState::MapInfo::T_WIN_BOMB},
			{{},                   GameState::MapInfo::MAP_ROUND_WIN_CAUSE_UNKNOWN}
	};

	const std::map<std::string, GameState::BombInfo::BombState> bombStateMapping = {
			{"planted", GameState::BombInfo::PLANTED},
			{{},        GameState::BombInfo::BOMB_STATE_UNKNOWN},
	};

	template<typename T>
	T getMapping(const nlohmann::json& json);

	template<>
	GameState::Side getMapping<GameState::Side>(const nlohmann::json& json);

	template<>
	GameState::Weapon::Type getMapping<GameState::Weapon::Type>(const nlohmann::json& json);

	template<>
	GameState::Weapon::State getMapping<GameState::Weapon::State>(const nlohmann::json& json);

	template<>
	GameState::GamePhase getMapping<GameState::GamePhase>(const nlohmann::json& json);

	template<>
	GameState::MapInfo::Mode getMapping<GameState::MapInfo::Mode>(const nlohmann::json& json);

	template<>
	GameState::MapInfo::RoundWinCause getMapping<GameState::MapInfo::RoundWinCause>(const nlohmann::json& json);

	template<>
	GameState::BombInfo::BombState getMapping<GameState::BombInfo::BombState>(const nlohmann::json& json);

	GameState::Provider parseToProvider(const nlohmann::json& json);

	GameState::MapInfo parseToMapInfo(const nlohmann::json& json);

	GameState::Weapon parseToWeapon(const nlohmann::json& json);

	GameState::Vec3 parseToVec3(const nlohmann::json& json);

	GameState::Player parseToPlayer(const nlohmann::json& json, const std::string& steamId);

	GameState::BombInfo parseToBombInfo(const nlohmann::json& json);

	GameState::MapInfo::SideStats parseToSideStats(const nlohmann::json& json, GameState::Side side);

	void parsePlayerState(const nlohmann::json& json, GameState::Player& player);

	void parsePlayerStats(const nlohmann::json& json, GameState::Player& player);

	int getOptionalInt(const nlohmann::json& json, const std::string& key);

public:
	GameState parse(nlohmann::json json);
};


#endif //GSIPACKETPARSER_H
