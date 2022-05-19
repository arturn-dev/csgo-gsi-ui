#ifndef GSIPACKETPARSER_H
#define GSIPACKETPARSER_H

#include <map>
#include <any>

#include "DisableMsvcWarnings.h"
#include <json.hpp>
#include "EnableMsvcWarnings.h"
#include "GameState.h"

#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Formatters/TxtFormatter.h>

class GSIPacketParser
{
	inline static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAdapter{};

	template<typename T>
	struct MapperTypeMap
	{
		// Mappings of strings from json to enum types (GameState class)
		static const std::map<std::string, T> mapper;
	};

	template<typename T>
	void setValueFromJson(T& destination, const nlohmann::json& json, const std::string& key);

	template<typename T>
	void setMappedValueFromJson(T& destination, const nlohmann::json& json, const std::string& key);

	std::vector<nlohmann::json> getVectorFromJson(const nlohmann::json& json, const std::string& key);

	std::map<nlohmann::json, nlohmann::json>
	getMapFromJson(const nlohmann::json& json);

	// TODO: Consider the idea of changing passing the 'json' object to every call by saving a reference to it as a class member.
	template<typename T>
	T getMapping(const nlohmann::json& json);

	GameState::MapInfo::SideStats getMapping(const nlohmann::json& json);

	int getOptionalInt(const nlohmann::json& json, const std::string& key);

public:
	GSIPacketParser();

	GameState parse(nlohmann::json json);
};

#endif //GSIPACKETPARSER_H
