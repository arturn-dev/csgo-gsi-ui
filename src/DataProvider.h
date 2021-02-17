#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include <list>
#include <unordered_map>
#include <json.hpp>

#include "DataType.h"

class IDataProviderListener;

class DataProvider
{
private:
	std::list<IDataProviderListener*> listeners;
	std::unordered_map<DataType, nlohmann::json> lastData;
	int lastUpdatedDataTypes = DataType::NONE;

	void notify();

public:
	void subscribe(IDataProviderListener* listener);
	void unsubscribe(IDataProviderListener* listener);
	void updateData(DataType dataType, nlohmann::json data);
};

class IDataProviderListener
{
public:
	virtual void update(DataType dataType, nlohmann::json data) = 0;
};

#endif //DATAPROVIDER_H
