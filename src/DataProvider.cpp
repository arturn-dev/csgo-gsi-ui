#include "DataProvider.h"

#include <utility>

void DataProvider::subscribe(IDataProviderListener* listener)
{
	listeners.push_back(listener);
}

void DataProvider::unsubscribe(IDataProviderListener* listener)
{
	listeners.remove(listener);
}

void DataProvider::notify()
{
	if (lastUpdatedDataTypes == NONE)
		return;

	for (int i = 0; i < DataType::COUNT; ++i)
	{
		auto dataType = static_cast<DataType>(lastUpdatedDataTypes & (1 << i));
		if (dataType != NONE)
			for (auto&& listener : listeners)
			{
				listener->update(lastData[dataType], dataType);
			}
	}

	lastUpdatedDataTypes = NONE;
}

void DataProvider::updateData(DataType dataType, nlohmann::json data)
{
	lastData[dataType] = std::move(data);
}