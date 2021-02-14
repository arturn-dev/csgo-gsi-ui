#include <iostream>

#include "GSIServer.h"

using std::cin;
using std::cout;

int main()
{
	cout << "Starting the server...\n";
	GSIServer server("127.0.0.1", 3000);
	cout << "Server started!\n";

	bool end = false;

	while (!end)
	{
		cout << "### Choose what to do:\n";
		cout << "### \t1. Get next data\n";
		cout << "### \t2. Finish\n";

		int c;
		cin >> c;
		switch (c)
		{
			case 1:
			{
				std::string data = server.getNextData();
				if (data.empty())
				{
					cout << "No data available.\n";
				}
				else
				{
					cout << "=== DATA START ===\n";
					cout << data << '\n';
					cout << "=== DATA END ===\n";
				}
			}
				break;
			case 2:
			{
				end = true;
			}
				break;
			default:
			{
				cout << "Unknown action.\n";
			}
		}
		cin.get();
	}

	cout << "Shutting down the server...\n";
	return 0;
}
