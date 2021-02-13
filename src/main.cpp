#include <iostream>
#include <httplib.h>

int main()
{
	httplib::Server server;

	server.Post("/", [](const httplib::Request& req, httplib::Response& res) {
		std::cout << "=== START OF REQUEST BODY ===\n" << req.body << "\n === END OF REQUEST BODY ===\n";
		res.set_content("{}", "application/json");
		res.status = 200;
	});

	server.listen("127.0.0.1", 3000);

	return 0;
}
