#include "GSIServer.h"

GSIServer::GSIServer(const std::string& host, int port)
{
	server.Post("/", [this](const httplib::Request& req, httplib::Response& res)
	{
		receiveData(req.body);
		res.set_content("{}", "application/json");
		res.status = 200;
	});

	listeningThread = std::thread([=]
								  {
									  server.listen(host.c_str(), port);
								  });

	while (!server.is_running())
		;
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
