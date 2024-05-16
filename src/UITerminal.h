#ifndef UITERMINAL_H
#define UITERMINAL_H

#include "UIInstance.h"

class UITerminal : public UIInstance
{
public:
	void init() override;

	void updateData(const GameState& gameState) override;
};


#endif //UITERMINAL_H
