#include <iostream>

#include "DataProvider.h"
#include "UIController.h"

using std::cin;
using std::cout;

int main()
{
	DataProvider dataProvider("127.0.0.1", 3000);
	UIController uiController;
	dataProvider.subscribe(&uiController);
	uiController.addUi();

	system("pause");

	return 0;
}
