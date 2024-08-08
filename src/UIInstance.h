#ifndef UIINSTANCE_H
#define UIINSTANCE_H

#include "GameState.h"

class UIInstance
{
public:
	virtual ~UIInstance();

	virtual void init() = 0;

	virtual void updateData(const GameState& gameState) = 0;
};


#endif //UIINSTANCE_H
