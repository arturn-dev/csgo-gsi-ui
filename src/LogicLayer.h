#ifndef LOGICLAYER_H
#define LOGICLAYER_H

#include "UIController.h"
#include "DataProvider.h"
#include "GSIServer.h"

class LogicLayer
{
	UIController uiController;
	DataProvider dataProvider;
	GSIServer gsiServer;

public:
	void init();
	void run();
};


#endif //LOGICLAYER_H
