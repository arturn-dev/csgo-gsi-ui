#ifndef IMGUIUI_H
#define IMGUIUI_H

#include "UIController.h"


class ImGuiUI : public UIImplementation
{
public:
	explicit ImGuiUI(IUIDataSource& dataSource);

	void run() override;
	void stop() override;
};


#endif //IMGUIUI_H
