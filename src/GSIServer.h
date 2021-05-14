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
	std::queue<std::string> dataQueue;

	std::thread listeningThread;
	std::mutex dataQueueMutex;
	std::condition_variable cv;

	void receiveData(const std::string& body);

public:
	GSIServer(const std::string& host, int port);
	virtual ~GSIServer();

	std::string getNextDataOrWait();
};


#endif //GSISERVER_H
