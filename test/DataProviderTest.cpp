#include "shared/GSIClientFixture.h"
#include "../src/DataProvider.h"
#include "GSIPacketParserTest.h"

class DataProviderTestListenerFixture
		: public GSIClientFixture // TODO: It should be a unit test and GSIServer should be mocked
{
protected:
	void SetUp() override
	{
		GSIClientFixture::SetUp();
	}

	void TearDown() override
	{
		GSIClientFixture::TearDown();
	}

	DataProvider dataProvider = DataProvider("127.0.0.1", port);
	GSIPacketParser gsiPacketParser;
};

struct DataProviderListener : public IDataProviderListener
{
	GameState data;
	std::atomic_bool updateInvoked = false;
	std::condition_variable cv;
	std::mutex m;

	void handleNewData(const GameState& data) override
	{
		updateInvoked = false;
		this->data = data;
		updateInvoked = true;
		cv.notify_all();
	}

	void waitForUpdate()
	{
		if (updateInvoked)
		{
			updateInvoked = false;
			return;
		}

		std::unique_lock<std::mutex> lock(m);
		cv.wait(lock);

		updateInvoked = false;
	}
};

TEST_F(DataProviderTestListenerFixture, ShouldReceiveAllData)
{
	DataProviderListener dataProviderListener;

	dataProvider.subscribe(&dataProviderListener);

	for (int i = 0; i < bodySamples.size(); i++)
	{
		sendNextPostRequest();
		dataProviderListener.waitForUpdate();

		auto originalData = gsiPacketParser.parse(nlohmann::json::parse(bodySamples[i]));
		assertGameState(dataProviderListener.data, originalData);
	}
}

TEST_F(DataProviderTestListenerFixture, ShouldReceiveOnlyFirstSample)
{
	DataProviderListener dataProviderListener;

	dataProvider.subscribe(&dataProviderListener);

	sendNextPostRequest();
	dataProviderListener.waitForUpdate();
	dataProvider.unsubscribe(&dataProviderListener);
	sendNextPostRequest();

	auto originalData = gsiPacketParser.parse(nlohmann::json::parse(bodySamples[0]));
	assertGameState(dataProviderListener.data, originalData);
}

TEST_F(DataProviderTestListenerFixture, ShouldReceiveSampleOnAllListeners)
{
	const int LISTENERS_COUNT = 10;
	std::vector<DataProviderListener> listeners(LISTENERS_COUNT);

	std::for_each(listeners.begin(), listeners.end(), [&](auto&& listener)
	{ dataProvider.subscribe(&listener); });
	sendNextPostRequest();
	auto originalData = gsiPacketParser.parse(nlohmann::json::parse(bodySamples[0]));
	for (auto& listener: listeners)
	{
		listener.waitForUpdate();
		assertGameState(listener.data, originalData);
	}
}

TEST_F(DataProviderTestListenerFixture, ShouldHandleNullptrWithoutException)
{
	dataProvider.subscribe(nullptr);
	sendNextPostRequest();
}