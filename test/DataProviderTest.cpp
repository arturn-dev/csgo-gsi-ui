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
	class DataProviderListener : public IDataProviderListener
	{
	public:
		nlohmann::json data;

		void update(const nlohmann::json& data, DataType dataType) override
		{
			this->data = data;
		}
	} dataProviderListener;

	dataProvider.subscribe(&dataProviderListener, DATA_RAW);

	sendNextPostRequest();

	ASSERT_EQ(dataProviderListener.data, bodySamples[0]);
}