#ifndef GSISERVER_H
#define GSISERVER_H

#include <vector>
#include <httplib.h>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

class GSIServer
{
	httplib::Server server;
	std::queue<std::string> data;

	std::thread t;
	std::mutex m;
	std::condition_variable cv;

public:
	GSIServer(const std::string& host, int port);
	virtual ~GSIServer();

	std::string getNextData();
};


#endif //GSISERVER_H
