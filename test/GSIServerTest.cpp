#include <future>

#include "shared/GSIClientFixture.h"


TEST_F(GSIClientFixture, ShouldTransferBodySampleFromClientToServer)
{
	auto server = std::make_unique<GSIServer>(address, port);

	sendNextPostRequest();

	ASSERT_STREQ(bodySamplesIt->c_str(), server->getNextDataOrWait().c_str());
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
		if (futureStatus != std::future_status::ready)
			FAIL() << "The server did not return the data.";

		if (result->status == 200)
			SUCCEED();
		else
			FAIL() << "Response from server was not '200'. 'Error' enum value is '" +
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

	for (int i = 0; i < bodySamples.size(); ++i)
	{
		std::future<std::string> f = std::async(std::launch::async, [&]
		{
			return server->getNextDataOrWait();
		});

		std::future_status futureStatus = f.wait_for(1s);
		if (futureStatus != std::future_status::ready)
			FAIL() << "The server did not return the data.";

		ASSERT_STREQ(f.get().c_str(), bodySamples[i].c_str());
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