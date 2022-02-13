#ifndef GSISERVER_H
#define GSISERVER_H

#include <vector>
#include <httplib.h>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Formatters/TxtFormatter.h>

class GSIServer
{
	httplib::Server server;
	std::queue<std::string> dataQueue;

	std::thread listeningThread;
	std::mutex dataQueueMutex;
	std::condition_variable cv;

	std::atomic_bool isStopping = false;

public:
	GSIServer(const std::string& host, int port);

	virtual ~GSIServer();

	std::string getNextDataOrWait();

	void stop();

private:

	void receiveData(const std::string& body);
};

static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAdapter;

#endif //GSISERVER_H
