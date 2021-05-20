#include <httplib.h>
#include <memory>
#include <fstream>
#include <string>
#include <future>
#include <filesystem>

#include "../src/GSIServer.h"
#include "gtest/gtest.h"



class GSIServerTestFixture : public ::testing::Test
{
protected:
	void SetUp() override
	{
		for (const auto& entry : std::filesystem::directory_iterator(dirPath))
		{
			const std::string filename = entry.path().string();
			std::ifstream ifs(filename);
			if(!ifs.good())
				FAIL() << "Couldn't open the file:  " + filename;

			std::stringstream ss;
			ss << ifs.rdbuf();
			bodySamples.push_back(ss.str());

			ifs.close();
		}

		bodySamplesIt = bodySamples.begin();

		client = std::make_unique<httplib::Client>(address, port);
		server = std::make_unique<GSIServer>(address, port);
	}

	void TearDown() override
	{

	}

	auto hasNextData()
	{
		return bodySamplesIt != bodySamples.end() - 1;
	}

	auto sendNextPostRequest()
	{
		if (firstSample)
			firstSample = false;
		else
			bodySamplesIt++;
		if (bodySamplesIt == bodySamples.end())
			throw std::logic_error("No more data available for a next POST sample request.");

		return client->Post("/", headers, *bodySamplesIt, "application/json");
	}

	std::unique_ptr<httplib::Client> client;
	std::unique_ptr<GSIServer> server;
	std::vector<std::string> bodySamples;
	std::vector<std::string>::iterator bodySamplesIt;
	bool firstSample = true;

	const std::string dirPath = "../test/data/body_samples";
	const std::string filenamePrefix = "sample";
	const std::string address = "127.0.0.1";
	const int port = 3001;
	const httplib::Headers headers = {
			{"user-agent", "Valve/Steam HTTP Client 1.0 (730)"},
			{"accept", "text/html*/*;q=0.9"},
			{"accept-encoding", "gzip,identity,*;q=0"},
			{"accept-charset", "ISO-8859-1,utf-8,*;q=0.7"}
	};

};

TEST_F(GSIServerTestFixture, ShouldTransferBodySampleFromClientToServer)
{
	sendNextPostRequest();

	ASSERT_STREQ(bodySamplesIt->c_str(), server->getNextDataOrWait().c_str());
}

TEST_F(GSIServerTestFixture, ShouldWaitUntilNewDataComes)
{
	using namespace std::chrono_literals;

	std::future<std::string> f = std::async(std::launch::async, [&]{
		return server->getNextDataOrWait();
	});

	if(auto result = sendNextPostRequest())
	{
		std::future_status futureStatus = f.wait_for(1s);
		if (futureStatus != std::future_status::ready)
			FAIL() << "The server did not return the data.";

		if(result->status == 200)
			SUCCEED();
		else
			FAIL() << "Response from server was not '200'. 'Error' enum value is '" + std::to_string(result.error()) + "'";
	}
}

TEST_F(GSIServerTestFixture, ShouldReceiveSameThatWasSent)
{
	using namespace std::chrono_literals;

	while(hasNextData())
	{
		auto result = sendNextPostRequest();
		ASSERT_EQ(result->status, 200);
	}

	for (int i = 0; i < bodySamples.size(); ++i)
	{
		std::future<std::string> f = std::async(std::launch::async, [&]{
			return server->getNextDataOrWait();
		});

		std::future_status futureStatus = f.wait_for(1s);
		if (futureStatus != std::future_status::ready)
			FAIL() << "The server did not return the data.";

		ASSERT_STREQ(f.get().c_str(), bodySamples[i].c_str());
	}
}

TEST_F(GSIServerTestFixture, Utility_GetSamplesAndSave)
{
	GTEST_SKIP();
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
		if(!ofs.good())
			FAIL() << "Couldn't open the file: " + filePath;
		ofs.write(data.c_str(), static_cast<long long>(data.size()));
		ofs.close();
	}
}