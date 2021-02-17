#include "UIController.h"
#include "ImGuiUI.h"

#include <shared_mutex>

void UIController::init()
{
	using namespace std;

	unique_ptr<UIImplementation> ui = make_unique<ImGuiUI>(*this);
	thread t(&ImGuiUI::run, &ui);

	uis.insert(pair<thread, unique_ptr<UIImplementation>>(move(t), move(ui)));
}

UIController::UIController()
{
}

void UIController::update(DataType dataType, nlohmann::json data)
{
	std::unique_lock<std::mutex> lock(m);
	lastData[dataType] = std::move(data);
}

nlohmann::json UIController::getData(DataType dataType)
{
	nlohmann::json res;

	std::shared_lock<std::mutex> lock(m);
	res = lastData[dataType];

	return res;
}


UIImplementation::UIImplementation(IUIDataSource& dataSource)
		: dataSource(dataSource)
{

}

void UIImplementation::notifyNewData(DataType dataType)
{
	newDataAvailability[dataType] = true;
}

void UIImplementation::updateDataCache()
{
	for (auto&& item : newDataAvailability)
	{
		if (item.second)
			dataCache[item.first] = dataSource.getData(item.first);
	}
}
