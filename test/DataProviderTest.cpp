#include "shared/GSIClientFixture.h"
#include "../src/DataProvider.h"

class DataProviderTestListenerFixture : public GSIClientFixture
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
};

struct DataProviderListener : public IDataProviderListener
{
	nlohmann::json data;
	std::atomic_bool updateInvoked = false;
	std::condition_variable cv;
	std::mutex m;

	void update(const nlohmann::json& data, DataType dataType) override
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

	dataProvider.subscribe(&dataProviderListener, DATA_RAW);

	for (int i = 0; i < bodySamples.size(); i++)
	{
		sendNextPostRequest();
		dataProviderListener.waitForUpdate();

		auto originalData = nlohmann::json::parse(bodySamples[i]);
		ASSERT_EQ(dataProviderListener.data, originalData);
	}
}

TEST_F(DataProviderTestListenerFixture, ShouldReceiveOnlyFirstSample)
{
	DataProviderListener dataProviderListener;

	dataProvider.subscribe(&dataProviderListener, DATA_RAW);

	sendNextPostRequest();
	dataProviderListener.waitForUpdate();
	dataProvider.unsubscribe(&dataProviderListener);
	sendNextPostRequest();

	auto originalData = nlohmann::json::parse(bodySamples[0]);
	ASSERT_EQ(dataProviderListener.data, originalData);
}

TEST_F(DataProviderTestListenerFixture, ShouldReceiveSampleOnAllListeners)
{
	const int LISTENERS_COUNT = 10;
	std::vector<DataProviderListener> listeners(LISTENERS_COUNT);

	std::for_each(listeners.begin(), listeners.end(), [&](auto&& listener)
	{ dataProvider.subscribe(&listener, DATA_RAW); });
	sendNextPostRequest();
	auto originalData = nlohmann::json::parse(bodySamples[0]);
	for (auto& listener: listeners)
	{
		listener.waitForUpdate();
		ASSERT_EQ(listener.data, originalData);
	}
}

TEST_F(DataProviderTestListenerFixture, ShouldHandleNullptrWithoutException)
{
	dataProvider.subscribe(nullptr, DATA_RAW);
	sendNextPostRequest();
}