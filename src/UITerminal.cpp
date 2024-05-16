#include "UITerminal.h"

#include <iostream>

void UITerminal::init()
{

}

void UITerminal::updateData(const GameState& gameState)
{
	std::cout << "timestamp: " << gameState.getProvider().timestamp << std::endl;
	std::cout << "countdown: " << gameState.getBombInfo().countdown << std::endl;
}
