#pragma once

#include "IApp.hpp"
#include "AppScreen.hpp"
#include "ScreenList.hpp"

class App : public vitiGL::IApp {
public:
	App		();

	App		(const std::string windowName, int width = 800, int height = 800,
			 vitiGL::wStyle style = vitiGL::wStyle::max, 
			 vitiGL::Color color = { 0x00, 0x00, 0x00, 0xFF });

	~App	();

	virtual void addScreens() override;

private:
	virtual void onInit() override;
	virtual void onExit() override;

	AppScreen* _screen;
};

