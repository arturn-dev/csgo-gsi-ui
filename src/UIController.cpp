#include "UIController.h"
#include "UITerminal.h"

UIInstance* UIController::addUi()
{
	uis.push_back(std::make_unique<UITerminal>());
	uis.back()->init();
	return uis.back().get();
}

void UIController::handleNewData(const GameState& data)
{
	std::for_each(uis.begin(), uis.end(), [&](auto& ui)
	{
		ui->updateData(data);
	});
}
