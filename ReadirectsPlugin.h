#pragma once
#pragma comment(lib, "pluginsdk.lib")
#include <chrono>
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"

#include "version.h"
constexpr auto plugin_version =
	stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

class ReadirectsPlugin : public BakkesMod::Plugin::BakkesModPlugin, public BakkesMod::Plugin::PluginSettingsWindow {
public:
	virtual void onLoad();
	virtual void onUnload();
	void				 HookGameEngine();
	void				 UnhookGameEngine();
	// primary driver
	void LaunchBall(std::shared_ptr<CVarManagerWrapper> &, std::shared_ptr<GameWrapper> &, std::vector<std::string>);

	// bakkesmod pluginmanager settings
	std::string GetPluginName() override;
	void				SetImGuiContext(uintptr_t ctx) override;
	void				RenderSettings() override;

private:
	// calculators / options
	void TowardsGoal();
	void TowardsWall();
	void TowardsCorner();
	void TowardsCeiling();
	void TowardsPlayer();

	void OnCarHitsBall(std::string eventName);
	void OnBallHitsWorld(std::string eventName);
	void OnBallHitsGround(std::string eventName);
	void OnGameTick(std::string eventName);

	// member variables
	int																		_launchBallTimer = 0;
	std::chrono::system_clock::time_point _lastPoint;
};
