#include <iostream>

#include "DataProvider.h"
#include "UIController.h"

#include <plog/Log.h>

using std::cin;
using std::cout;

auto maxSev = plog::Severity::debug;

int main(int argc, char** argv)
{
	if (argc == 2)
	{
		maxSev = plog::severityFromString(argv[1]);
	}

	DataProvider dataProvider("127.0.0.1", 3000);
	UIController uiController;
	dataProvider.subscribe(&uiController);
	uiController.addUi();

	cin.get();

	return 0;
}
