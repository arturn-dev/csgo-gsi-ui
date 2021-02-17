#include "LogicLayer.h"
#include "ImGuiUI.h"

void LogicLayer::init()
{
	dataProvider.subscribe(&uiController);
}

void LogicLayer::run()
{

}
