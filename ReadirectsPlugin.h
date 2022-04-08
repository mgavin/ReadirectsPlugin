#pragma once
#pragma comment(lib, "pluginsdk.lib")
#include <chrono>
#include <deque>
#include <string>
#include <vector>
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "imgui.h"

#include "version.h"
constexpr auto plugin_version =
  stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

class ReadirectsPlugin : public BakkesMod::Plugin::BakkesModPlugin, public BakkesMod::Plugin::PluginSettingsWindow {
public:
  virtual void onLoad();
  virtual void onUnload();
  void         HookGameEngine();
  void         UnhookGameEngine();
  // primary driver
  void LaunchBall(std::vector<std::string>);

  // bakkesmod pluginmanager settings
  std::string GetPluginName() override;
  void        SetImGuiContext(uintptr_t ctx) override;
  void        RenderSettings() override;

private:
  // calculators / options
  void TowardsGoal(std::shared_ptr<ServerWrapper> sw, std::shared_ptr<CarWrapper> player);
  void TowardsWall(std::shared_ptr<ServerWrapper> sw, std::shared_ptr<CarWrapper> player);
  void TowardsCorner(std::shared_ptr<ServerWrapper> sw, std::shared_ptr<CarWrapper> player);
  void TowardsCeiling(std::shared_ptr<ServerWrapper> sw, std::shared_ptr<CarWrapper> player);
  void TowardsCar(std::shared_ptr<ServerWrapper> sw, std::shared_ptr<CarWrapper> player);

  void OnCarHitsBall(std::string eventName);
  void OnBallHitsWorld(std::string eventName);
  void OnBallHitsGround(std::string eventName);
  void OnGameTick(std::string eventName);

  std::string next_in_playlist();

  // member variables
  int                                   _launchBallTimer = 0;
  std::chrono::system_clock::time_point _lastPoint;
  std::deque<std::string>               playlist;
  // holds a copy of the playlist for execution
  std::deque<std::string> c_playlist;

  // for collapse all / expand all in the menu
  ImGuiStorage *       settings_storage;
  std::vector<ImGuiID> settings_ids;
};
