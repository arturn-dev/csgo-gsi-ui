#include "GSIServer.h"

GSIServer::GSIServer(const std::string& host, int port)
{
	server.Post("/", [this](const httplib::Request& req, httplib::Response& res) {
		receiveData(req.body);
		res.set_content("{}", "application/json");
		res.status = 200;
	});

	listeningThread = std::thread([=]{
		server.listen(host.c_str(), port);
	});
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
	std::string ret;

	std::unique_lock<std::mutex> lock(dataQueueMutex);
	if (dataQueue.empty())
		// No data available. Block thread while waiting for new data.
		cv.wait(lock);

	ret = dataQueue.front();
	dataQueue.pop();

	return ret;
}

GSIServer::~GSIServer()
{
	server.stop();
	listeningThread.join();
}
