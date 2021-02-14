#include "GSIServer.h"

GSIServer::GSIServer(const std::string& host, int port)
{
	server.Post("/", [this](const httplib::Request& req, httplib::Response& res) {
		bool queueWasEmpty = false;
		{
			std::lock_guard<std::mutex> lock(m);
			if (data.empty())
				queueWasEmpty = true;
			data.push(req.body);
		}
		if (queueWasEmpty)
			cv.notify_one(); // notify a thread that waits for new data

		res.set_content("{}", "application/json");
		res.status = 200;
	});

	t = std::thread([=]{
		server.listen(host.c_str(), port);
	});
}

std::string GSIServer::getNextData()
{
	std::string ret;

	std::unique_lock<std::mutex> lock(m);
	if (data.empty())
		// No data available. Block thread while waiting for new data.
		cv.wait(lock);

	ret = data.front();
	data.pop();

	return ret;
}

GSIServer::~GSIServer()
{
	server.stop();
	t.join();
}
