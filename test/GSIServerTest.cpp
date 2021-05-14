#include <httplib.h>
#include <memory>
#include <fstream>
#include <iostream>
#include <string>

#include "../src/GSIServer.h"
#include "gtest/gtest.h"

class GSIServerTestFixture : public ::testing::Test
{
protected:
	void SetUp() override
	{
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
				{"Accept", "text/html*/*;q=0.9"},
				{"accept-encoding", "gzip,identity,*;q=0"},
				{"accept-charset", "ISO-8859-1,utf-8,*;q=0.7"}
		};

		return client->Post("/", headers, body, "application/json");
	}

	std::unique_ptr<httplib::Client> client;
	std::unique_ptr<GSIServer> server;
};

TEST_F(GSIServerTestFixture, TestCase1)
{
	std::ifstream ifs("../test/post_body_sample.txt");
	std::stringstream ss;
	ss << ifs.rdbuf();
	std::string fileContent = ss.str();

	sendPostRequest(fileContent);

	ASSERT_STREQ(fileContent.c_str(), server->getNextDataOrWait().c_str());
}