#include "DataProvider.h"

DataProvider::DataProvider(const std::string& host, int port) : _gsiServer(host, port), isBeingDestroyed(false)
{
	dataFetchThread = std::thread([&]
								  {
									  while (!isBeingDestroyed)
									  {
										  auto data = _gsiServer.getNextDataOrWait();
										  if (!data.empty())
											  notify(nlohmann::json::parse(data), DATA_RAW);
									  }
								  });
}

DataProvider::~DataProvider()
{
	_gsiServer.stop();
	isBeingDestroyed = true;
	dataFetchThread.join();
}

void DataProvider::subscribe(IDataProviderListener* listener, DataType dataType)
{
	if (listener != nullptr)
		listeners.emplace(dataType, listener);
}

void DataProvider::unsubscribe(IDataProviderListener* listener)
{
	if (listener == nullptr) return;
	
	auto predicate = [listener](const std::pair<DataType, IDataProviderListener*>& entry)
	{ return entry.second == listener; };
	auto foundEntryIt = std::find_if(listeners.begin(), listeners.end(), predicate);
	while (foundEntryIt != listeners.end())
	{
		auto nextIt = listeners.erase(foundEntryIt);
		foundEntryIt = std::find_if(nextIt, listeners.end(), predicate);
	}
}

void DataProvider::notify(const nlohmann::json& data, DataType dataType)
{
	auto bucketIndex = listeners.bucket(dataType);
	std::for_each(listeners.begin(bucketIndex), listeners.end(bucketIndex),
				  [&](auto& listener)
				  { listener.second->update(data, dataType); });
}
