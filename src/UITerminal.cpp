#include "UITerminal.h"

#include <iostream>
#include <iomanip>

void UITerminal::init()
{

}

void UITerminal::clearDisplay()
{
	printf("%c[D", 0x1B); // Helps to remove flickering on Windows
	printf("%c[J", 0x1B); // Clear display from cursor to the end
}

void UITerminal::resetCursor()
{
	printf("%c[%dF", 0x1B, linesCount); // Set cursor to the beginning
	linesCount = 0;
}

void UITerminal::flush()
{
	std::cout << output.rdbuf();
}

void UITerminal::updateData(const GameState& gameState)
{
	append("=== Provider ===");
	append("Name", gameState.getProvider().name);
	append("Version", gameState.getProvider().version);
	append("App ID", gameState.getProvider().appId);
	append("Steam ID", gameState.getProvider().steamId);
	append("Time", gameState.getProvider().timestamp);
	append("================");
	append("=== Game State ===");
	append("Map", gameState.getMapInfo().name);
	append("Round", gameState.getMapInfo().roundNo + 1);
	append("Bomb", toString(gameState.getBombInfo().bombState), ' ');
	if ((gameState.getBombInfo().bombState == GameState::BombInfo::PLANTED ||
		 gameState.getBombInfo().bombState == GameState::BombInfo::PLANTING ||
		 gameState.getBombInfo().bombState == GameState::BombInfo::DEFUSING) &&
		gameState.getBombInfo().countdown >= 0)
	{
		std::cout << "(" << std::fixed << std::setprecision(1) << gameState.getBombInfo().countdown << ")";
	}
	append();
	append();
	append("Terrorists");
	append("\tScore", gameState.getMapInfo().tSideStats.score);
	append("Counter-Terrorists");
	append("\tScore", gameState.getMapInfo().ctSideStats.score);
	append("==================");

	clearDisplay();
	flush();
	resetCursor();
}

void UITerminal::append(const std::string& str, const char end)
{
	output << str << end;
	if (end == '\n')
	{
		linesCount++;
	}
}

template<typename T>
void UITerminal::append(const std::string& label, const T& value, const char end)
{
	output << label << ": " << value << end;
	if (end == '\n')
	{
		linesCount++;
	}
}

std::string UITerminal::toString(GameState::BombInfo::BombState enumValue)
{
	switch (enumValue)
	{
		case GameState::BombInfo::CARRIED:
			return "Carried";
		case GameState::BombInfo::DROPPED:
			return "Dropped";
		case GameState::BombInfo::PLANTING:
			return "Planting";
		case GameState::BombInfo::PLANTED:
			return "Planted";
		case GameState::BombInfo::DEFUSING:
			return "Defusing";
		case GameState::BombInfo::DEFUSED:
			return "Defused";
		case GameState::BombInfo::BOMB_STATE_UNKNOWN:
			return "State unknown (" + std::to_string(enumValue) + ")";
	}
	return "State unknown (" + std::to_string(enumValue) + ")";
}

UITerminal::~UITerminal()
{
	clearDisplay();
}
