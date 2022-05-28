#include <future>

#include "shared/GSIClientFixture.h"


TEST_F(GSIClientFixture, ShouldTransferBodySampleFromClientToServer)
{
	auto server = std::make_unique<GSIServer>(address, port);

	sendNextPostRequest();

	ASSERT_EQ(*bodySamplesIt, server->getNextDataOrWait());
}

TEST_F(GSIClientFixture, ShouldWaitUntilNewDataComes)
{
	auto server = std::make_unique<GSIServer>(address, port);

	using namespace std::chrono_literals;

	std::future<std::string> f = std::async(std::launch::async, [&]
	{
		return server->getNextDataOrWait();
	});

	if (auto result = sendNextPostRequest())
	{
		std::future_status futureStatus = f.wait_for(1s);
		ASSERT_TRUE(futureStatus == std::future_status::ready) << "The server did not return the data.";
		ASSERT_TRUE(result->status == 200) << "Response from server was not '200'. 'Error' enum value is '" +
											  std::to_string((int) result.error()) +
											  "'";
	}
}

TEST_F(GSIClientFixture, ShouldReceiveSameThatWasSent)
{
	auto server = std::make_unique<GSIServer>(address, port);

	using namespace std::chrono_literals;

	while (hasNextData())
	{
		auto result = sendNextPostRequest();
		ASSERT_EQ(result->status, 200);
	}

	for (const auto& bodySample: bodySamples)
	{
		std::future<std::string> f = std::async(std::launch::async, [&]
		{
			return server->getNextDataOrWait();
		});

		std::future_status futureStatus = f.wait_for(1s);
		ASSERT_TRUE(futureStatus == std::future_status::ready) << "The server did not return the data.";
		ASSERT_EQ(f.get(), bodySample);
	}
}