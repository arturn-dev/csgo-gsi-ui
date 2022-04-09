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

TEST_F(GSIClientFixture, Utility_GetSamplesAndSave)
{
	GTEST_SKIP();
	auto server = std::make_unique<GSIServer>(address, port);

	const int SAMPLES_COUNT = 10;

	for (int i = 0; i < SAMPLES_COUNT; ++i)
	{
		const std::string filePath = std::string(dirPath)
				.append("/")
				.append(filenamePrefix)
				.append(std::to_string(i))
				.append(".txt");
		const std::string data = server->getNextDataOrWait();

		std::ofstream ofs(filePath);
		if (!ofs.good())
			FAIL() << "Couldn't open the file: " + filePath;
		ofs.write(data.c_str(), static_cast<long long>(data.size()));
		ofs.close();
	}
}