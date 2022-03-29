#pragma once
#pragma comment(lib, "pluginsdk.lib")
#include <chrono>
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(
	VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

class ReadirectsPlugin :
		public BakkesMod::Plugin::BakkesModPlugin,
		public BakkesMod::Plugin::PluginSettingsWindow {
public:
	virtual void onLoad();
	virtual void onUnload();

	// primary driver
	void launchBall();

	// settings
	std::string GetPluginName() override;
	void				SetImGuiContext(uintptr_t ctx) override;
	void				RenderSettings() override;

private:
	// calculators / options
	// towards goal
	// towards player
	// towards wall
	void onCarHitBall(std::string eventName);
	void onWorldHitBall(std::string eventName);
	void onGameTick(std::string eventName);

	// accessors
	// void getSettings() const;

	// member variables
	int																		_launchBallTimer = 0;
	std::chrono::system_clock::time_point _lastPoint;
};
