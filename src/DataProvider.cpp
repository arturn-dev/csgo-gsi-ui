#include "DataProvider.h"

DataProvider::DataProvider(const std::string& host, int port) : _gsiServer(host, port), isBeingDestroyed(false)
{
	dataFetchThread = std::thread([&]
								  {
									  while (!isBeingDestroyed)
									  {
										  auto data = _gsiServer.getNextDataOrWait();
										  if (!data.empty())
										  {
											  notify(nlohmann::json::parse(data));
										  }
									  }
								  });
}

DataProvider::~DataProvider()
{
	_gsiServer.stop();
	isBeingDestroyed = true;
	dataFetchThread.join();
}

void DataProvider::subscribe(IDataProviderListener* listener)
{
	if (listener != nullptr)
	{
		listeners.insert(listener);
	}
}

void DataProvider::unsubscribe(IDataProviderListener* listener)
{
	if (listener == nullptr)
	{
		return;
	}

	auto foundEntryIt = std::find(listeners.begin(), listeners.end(), listener);
	while (foundEntryIt != listeners.end())
	{
		auto nextIt = listeners.erase(foundEntryIt);
		foundEntryIt = std::find(nextIt, listeners.end(), listener);
	}
}

void DataProvider::notify(const nlohmann::json& data)
{
	auto gameState = gsiPacketParser.parse(data);
	std::for_each(listeners.begin(), listeners.end(),
				  [&](auto& listener)
				  { listener->handleNewData(gameState); });
}
