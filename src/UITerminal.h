#ifndef UITERMINAL_H
#define UITERMINAL_H

#include <sstream>

#include "UIInstance.h"

class UITerminal : public UIInstance
{
	int linesCount = 0;
	std::stringstream output;

	void clearDisplay();

	void resetCursor();

	void flush();

	template<typename T>
	void append(const std::string& label, const T& value, const char end = '\n');

	void append(const std::string& str = "", const char end = '\n');

	std::string toString(GameState::BombInfo::BombState enumValue);

public:
	~UITerminal() override;

	void init() override;

	void updateData(const GameState& gameState) override;
};


#endif //UITERMINAL_H
