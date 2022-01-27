#ifndef GSICLIENTFIXTURE_H
#define GSICLIENTFIXTURE_H

#include <memory>
#include <fstream>
#include <string>
#include <filesystem>

#include <httplib.h>

#include "gtest/gtest.h"
#include "../../src/GSIServer.h"

class GSIClientFixture : public ::testing::Test
{
protected:
	void SetUp() override
	{
		for (const auto& entry: std::filesystem::directory_iterator(dirPath))
		{
			const std::string filename = entry.path().string();
			std::ifstream ifs(filename);
			if (!ifs.good())
				FAIL() << "Couldn't open the file:  " + filename;

			std::stringstream ss;
			ss << ifs.rdbuf();
			bodySamples.push_back(ss.str());

			ifs.close();
		}

		bodySamplesIt = bodySamples.begin();

		client = std::make_unique<httplib::Client>(address, port);
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
	std::vector<std::string> bodySamples;
	std::vector<std::string>::iterator bodySamplesIt;
	bool firstSample = true;

	const std::string dirPath = "../test/data/body_samples";
	const std::string filenamePrefix = "sample";
	const std::string address = "0.0.0.0";
	const int port = 3000;
	const httplib::Headers headers = {
			{"user-agent",      "Valve/Steam HTTP Client 1.0 (730)"},
			{"accept",          "text/html*/*;q=0.9"},
			{"accept-encoding", "gzip,identity,*;q=0"},
			{"accept-charset",  "ISO-8859-1,utf-8,*;q=0.7"}
	};

};

#endif //GSICLIENTFIXTURE_H
