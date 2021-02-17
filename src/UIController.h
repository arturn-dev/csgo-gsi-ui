#ifndef UICONTROLLER_H
#define UICONTROLLER_H

#include <thread>
#include <memory>
#include <mutex>
#include "DataProvider.h"

class UIImplementation;

class IUIDataSource
{
public:
	virtual nlohmann::json getData(DataType dataType) = 0;
};

class UIController : public IDataProviderListener, public IUIDataSource
{
	std::unordered_map<std::thread, std::unique_ptr<UIImplementation>> uis;
	std::unordered_map<DataType, nlohmann::json> lastData;
	std::mutex m;

	void init();
public:
	UIController();

	void update(DataType dataType, nlohmann::json data) override;

	nlohmann::json getData(DataType dataType) override;
};

class UIImplementation
{
protected:
	IUIDataSource& dataSource;
	std::unordered_map<DataType, bool> newDataAvailability;
	std::unordered_map<DataType, nlohmann::json> dataCache;

	void updateDataCache();

public:
	explicit UIImplementation(IUIDataSource& dataSource);

	virtual void run() = 0;
	virtual void stop() = 0;
	void notifyNewData(DataType dataType);
};

#endif //UICONTROLLER_H
