#include <httplib.h>
#include <memory>
#include <fstream>
#include <iostream>
#include <string>
#include <future>

#include "../src/GSIServer.h"
#include "gtest/gtest.h"



class GSIServerTestFixture : public ::testing::Test
{
protected:
	void SetUp() override
	{
		const std::string filename = "../test/post_body_sample.txt";
		std::ifstream ifs(filename);
		if(!ifs.good())
			FAIL() << "Couldn't load POST body sample from " + filename;
		std::stringstream ss;
		ss << ifs.rdbuf();
		postBodySample = ss.str();

		client = std::make_unique<httplib::Client>("127.0.0.1", 3000);
		server = std::make_unique<GSIServer>("127.0.0.1", 3000);


	}

	void TearDown() override
	{

	}

	auto sendPostRequest(const std::string& body)
	{
		httplib::Headers headers = {
				{"user-agent", "Valve/Steam HTTP Client 1.0 (730)"},
				{"accept", "text/html*/*;q=0.9"},
				{"accept-encoding", "gzip,identity,*;q=0"},
				{"accept-charset", "ISO-8859-1,utf-8,*;q=0.7"}
		};

		return client->Post("/", headers, body, "application/json");
	}

	std::unique_ptr<httplib::Client> client;
	std::unique_ptr<GSIServer> server;
	std::string postBodySample;
};

TEST_F(GSIServerTestFixture, ShouldTransferBodySampleFromClientToServer)
{
	sendPostRequest(postBodySample);

	ASSERT_STREQ(postBodySample.c_str(), server->getNextDataOrWait().c_str());
}

TEST_F(GSIServerTestFixture, ShouldWaitUntilNewDataComes)
{
	using namespace std::chrono_literals;

	std::future<std::string> f = std::async(std::launch::async, [&]{
		return server->getNextDataOrWait();
	});

	if(auto result = sendPostRequest(postBodySample))
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

