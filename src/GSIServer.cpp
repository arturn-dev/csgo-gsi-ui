#include "GSIServer.h"
#include <plog/Init.h>
#include <plog/Log.h>

GSIServer::GSIServer(const std::string& host, int port)
{
	plog::init(plog::debug, &consoleAdapter);

	server.Post("/", [this](const httplib::Request& req, httplib::Response& res)
	{
		receiveData(req.body);
		res.set_content("{}", "application/json");
		res.status = 200;
	});

	listeningThread = std::thread([=]
								  {
									  bool success = server.listen(host.c_str(), port);
									  if (!success)
										  LOG(plog::error) << "Could not start GSI server";
								  });

	const int SERVER_START_TIMEOUT_SECS = 3;
	LOG(plog::info) << ("Waiting " + std::to_string(SERVER_START_TIMEOUT_SECS) + " seconds for server start...");
	auto startTime = std::chrono::high_resolution_clock::now();

	while (!server.is_running())
	{
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(100ms);

		if (listeningThread.joinable())
		{
			listeningThread.join();
			break;
		}

		if (std::chrono::duration_cast<std::chrono::seconds>(
				std::chrono::high_resolution_clock::now() - startTime).count() > SERVER_START_TIMEOUT_SECS &&
			!server.is_running())
		{
			LOG(plog::error) << "Server initialization timed out. Unable to start the GSI server.";
			break;
		}
	}

	if (server.is_running())
		LOG(plog::info) << ("GSI server started. Listening on " + host + ":" + std::to_string(port));
}

GSIServer::~GSIServer()
{
	stop();
}

void GSIServer::receiveData(const std::string& body)
{
	bool queueWasEmpty = false;
	{
		std::lock_guard<std::mutex> lock(dataQueueMutex);
		if (dataQueue.empty())
			queueWasEmpty = true;
		dataQueue.push(body);
	}
	if (queueWasEmpty)
		cv.notify_one(); // notify a thread that waits for new data
}

std::string GSIServer::getNextDataOrWait()
{
	if (isStopping)
		return "";

	std::string ret;

	std::unique_lock<std::mutex> lock(dataQueueMutex);
	if (dataQueue.empty())
		// No data available. Block thread while waiting for new data.
		cv.wait(lock);

	if (!dataQueue.empty())
	{
		ret = dataQueue.front();
		dataQueue.pop();
	}

	return ret;
}

void GSIServer::stop()
{
	isStopping = true;
	cv.notify_all();
	if (server.is_running())
		server.stop();
	listeningThread.join();
}
