#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include <list>
#include <set>

#include "DisableMsvcWarnings.h"
#include <json.hpp>
#include "EnableMsvcWarnings.h"

#include "GSIServer.h"
#include "GSIPacketParser.h"

class IDataProviderListener;

class DataProvider
{
private:
	std::set<IDataProviderListener*> listeners;
	std::thread dataFetchThread;
	GSIServer _gsiServer;
	GSIPacketParser gsiPacketParser;


	std::atomic_bool isBeingDestroyed;

public:
	DataProvider(const std::string& host, int port);

	virtual ~DataProvider();

	void subscribe(IDataProviderListener* listener);

	void unsubscribe(IDataProviderListener* listener);

private:
	void notify(const nlohmann::json& data);
};

class IDataProviderListener
{
public:
	virtual void handleNewData(const GameState& data) = 0;
};

#endif //DATAPROVIDER_H
