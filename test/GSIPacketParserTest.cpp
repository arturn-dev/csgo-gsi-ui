#include <fstream>
#include "gtest/gtest.h"
#include "gtest/gtest-printers.h"
#include "../../src/GSIPacketParser.h"

TEST(GSIPacketParser, x)
{
	const std::string filename = "../test/data/player.json";
	std::ifstream ifs(filename);
	if (!ifs.good())
		FAIL() << "Couldn't open the file:  " + filename;

	std::stringstream ss;
	ss << ifs.rdbuf();
	auto json = nlohmann::json::parse(ss.str());
	GSIPacketParser parser;
	auto gameState = parser.parse(json);
}