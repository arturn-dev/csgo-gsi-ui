#include "DataProvider.h"

DataProvider::DataProvider() : _gsiServer("127.0.0.1", 3000), isBeingDestroyed(false)
{
	dataFetchThread = std::thread([&]
								  {
									  while(!isBeingDestroyed)
									  {
										  auto data = _gsiServer.getNextDataOrWait();
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
	listeners.emplace(dataType, listener);
}

void DataProvider::unsubscribe(IDataProviderListener* listener)
{
	// TODO: implement
}

void DataProvider::notify(const nlohmann::json& data, DataType dataType)
{
	auto bucketIndex = listeners.bucket(dataType);
	std::for_each(listeners.begin(bucketIndex), listeners.end(bucketIndex),
				  [&](IDataProviderListener*& listener)
				  { listener->update(data, dataType); });
}
