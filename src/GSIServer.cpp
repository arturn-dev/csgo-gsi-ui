#include "GSIServer.h"
#include <plog/Init.h>
#include <plog/Log.h>

extern plog::Severity maxSev;

GSIServer::GSIServer(const std::string& host, int port)
{
	plog::init(maxSev, &consoleAppender);

	server.Post("/", [this](const httplib::Request& req, httplib::Response& res)
	{
		receiveData(req.body);
		res.set_content("{}", "application/json");
		res.status = 200;
	});

	listeningThread = std::thread([=]
								  {
									  server.listen(host.c_str(), port);
									  LOG(plog::info) << "GSI server stopped.";
								  });

	const int SERVER_START_TIMEOUT_SECS = 3;
	LOG(plog::info) << ("Waiting " + std::to_string(SERVER_START_TIMEOUT_SECS) + " seconds for server start...");
	auto startTime = std::chrono::high_resolution_clock::now();

	while (!server.is_running())
	{
		using namespace std::chrono_literals;
		using namespace std::chrono;

		std::this_thread::sleep_for(100ms);

		if (duration_cast<seconds>(
				high_resolution_clock::now() - startTime).count() > SERVER_START_TIMEOUT_SECS &&
			!server.is_running())
		{
			throw std::logic_error("Could not start the GSI server. Timeout.");
		}
	}

	if (server.is_running())
	{
		LOG(plog::info) << ("GSI server started. Listening on " + host + ":" + std::to_string(port));
	}
}

GSIServer::~GSIServer()
{
	if (!isStopping)
	{
		stop();
	}
}

void GSIServer::receiveData(const std::string& body)
{
	bool queueWasEmpty = false;
	{
		std::lock_guard<std::mutex> lock(dataQueueMutex);
		if (dataQueue.empty())
		{
			queueWasEmpty = true;
		}
		dataQueue.push(body);
	}
	if (queueWasEmpty)
	{
		cv.notify_one(); // notify a thread that waits for new data
	}
}

std::string GSIServer::getNextDataOrWait()
{
	if (isStopping)
	{
		return "";
	}

	std::string ret;

	std::unique_lock<std::mutex> lock(dataQueueMutex);
	if (dataQueue.empty())
	{
		// No data available. Block current thread while waiting for new data.
		cv.wait(lock);
	}

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
	LOG(plog::info) << "Stopping GSI server...";
	cv.notify_all();
	if (server.is_running())
	{
		server.stop();
	}
	listeningThread.join();
}
