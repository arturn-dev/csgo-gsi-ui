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

	DataProvider dataProvider = DataProvider("127.0.0.1", 3001);
};

TEST_F(DataProviderTestListenerFixture, ShouldReceiveAllData)
{
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
	} dataProviderListener;

	dataProvider.subscribe(&dataProviderListener, DATA_RAW);

	sendNextPostRequest();
	dataProviderListener.waitForUpdate();

	auto originalData = nlohmann::json::parse(bodySamples[0]);
	ASSERT_EQ(dataProviderListener.data, originalData);
}