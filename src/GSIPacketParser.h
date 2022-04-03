#ifndef GSIPACKETPARSER_H
#define GSIPACKETPARSER_H

#include <map>
#include <any>

#include "DisableMsvcWarnings.h"
#include <json.hpp>
#include "EnableMsvcWarnings.h"
#include "GameState.h"

class GSIPacketParser
{
	template<typename T>
	struct MapperTypeMap
	{
		static const std::map<std::string, T>& mapper;
	};

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
