#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include <list>
#include <unordered_map>
#include <json.hpp>

#include "DataType.h"
#include "GSIServer.h"

class IDataProviderListener;

class DataProvider
{
private:
	std::unordered_multimap<DataType, IDataProviderListener*> listeners;
	std::thread dataFetchThread;
	GSIServer _gsiServer;

	std::atomic_bool isBeingDestroyed;

public:
	DataProvider(const std::string& host, int port);

	virtual ~DataProvider();

	void subscribe(IDataProviderListener* listener, DataType dataType);

	void unsubscribe(IDataProviderListener* listener);

private:
	void notify(const nlohmann::json& data, DataType dataType);
};

class IDataProviderListener
{
public:
	virtual void update(const nlohmann::json& data, DataType dataType) = 0;
};

#endif //DATAPROVIDER_H
