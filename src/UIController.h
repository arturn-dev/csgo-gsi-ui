#ifndef UICONTROLLER_H
#define UICONTROLLER_H

#include "DataProvider.h"
#include "UIInstance.h"

class UIController : public IDataProviderListener
{
	std::vector<std::unique_ptr<UIInstance>> uis;

public:
	UIInstance* addUi();

	void handleNewData(const GameState& data) override;
};


#endif //UICONTROLLER_H
