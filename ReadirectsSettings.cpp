
// #include <Xinput.h>
#include "ReadirectsPlugin.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_rangeslider.h"
/*
 * use xinput to capture controller input?
 */

static const std::vector<std::string> KEY_LIST = {"XboxTypeS_A",
                                                  "XboxTypeS_B",
                                                  "XboxTypeS_X",
                                                  "XboxTypeS_Y",
                                                  "XboxTypeS_RightShoulder",
                                                  "XboxTypeS_RightTrigger",
                                                  "XboxTypeS_RightThumbStick",
                                                  "XboxTypeS_LeftShoulder",
                                                  "XboxTypeS_LeftTrigger",
                                                  "XboxTypeS_LeftThumbStick",
                                                  "XboxTypeS_Start",
                                                  "XboxTypeS_Back",
                                                  "XboxTypeS_DPad_Up",
                                                  "XboxTypeS_DPad_Left",
                                                  "XboxTypeS_DPad_Right",
                                                  "XboxTypeS_DPad_Down"};

std::string ReadirectsPlugin::GetPluginName() {
  return "Readirects Plugin";
}

void ReadirectsPlugin::SetImGuiContext(uintptr_t ctx) {
  ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext *>(ctx));
}

void ReadirectsPlugin::RenderSettings() {
  // helper functions to add ImGui elements
  auto addCheckbox = [this](std::string cvarName, const char * chkboxText, const char * tooltip) {
    CVarWrapper cvar = cvarManager->getCvar(cvarName);
    if (!cvar)
      return;

    bool enabled = cvar.getBoolValue();
    if (ImGui::Checkbox(chkboxText, &enabled)) {
      cvar.setValue(enabled);
    }

    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip(tooltip);
    }
  };
  auto addRangeSliderInt = [this](std::string cvarSuffix, const char * label) {
    CVarWrapper cvar = cvarManager->getCvar("readirects_" + cvarSuffix);
    if (!cvar)
      return;
    int         min, max;
    std::string values = cvar.getStringValue();
    sscanf(values.c_str(), "(%d, %d)", &min, &max);
    ImGui::RangeSliderInt(label, &min, &max, cvar.GetMinimum(), cvar.GetMaximum());
    std::string value = "(" + std::to_string(min) + ", " + std::to_string(max) + ")";
    cvar.setValue(value);
  };
  auto addRangeSliderFloat = [this](std::string cvarSuffix, const char * label) {
    CVarWrapper cvar = cvarManager->getCvar("readirects_" + cvarSuffix);
    if (!cvar)
      return;
    float       min, max;
    std::string values = cvar.getStringValue();
    sscanf(values.c_str(), "(%f, %f)", &min, &max);
    ImGui::RangeSliderFloat(label, &min, &max, cvar.GetMinimum(), cvar.GetMaximum());
    std::string value = "(" + std::to_string(min) + ", " + std::to_string(max) + ")";
    cvar.setValue(value);
  };
  /* FIRST LINE */
  ImGui::Spacing();
  ImGui::AlignTextToFramePadding();
  // enable plugin checkbox
  addCheckbox("readirects_enabled", "Enable Plugin", "Toggle Readirects Plugin");
  ImGui::SameLine();
  ImGui::SameLine(300);

  if (ImGui::Button("+ Expand Settings")) {
    if (settings_storage != NULL) {
      for (auto id : settings_ids) {
        settings_storage->SetInt(id, 1);
      }
    }
  }
  ImGui::SameLine();
  if (ImGui::Button("- Collapse Settings")) {
    if (settings_storage != NULL) {
      for (auto id : settings_ids) {
        settings_storage->SetInt(id, 0);
      }
    }
  }

  /* END FIRST LINE */

  /* SECOND LINE */
  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();
  /* END SECOND LINE */

  /* THIRD LINE */
  ImGui::AlignTextToFramePadding();
  ImGui::TextUnformatted("Enable ball redirecting towards: ");
  ImGui::SameLine();
  // clang-format off
  addCheckbox("readirects_towards_goal",    "Goal",    "Toggle ball redirecting towards goal");
  ImGui::SameLine();
  addCheckbox("readirects_towards_wall",    "Wall",    "Toggle ball redirecting towards wall");
  ImGui::SameLine();
  addCheckbox("readirects_towards_corner",  "Corner",  "Toggle ball redirecting towards corner");
  ImGui::SameLine();
  addCheckbox("readirects_towards_ceiling", "Ceiling", "Toggle ball redirecting towards ceiling");
  ImGui::SameLine();
  addCheckbox("readirects_towards_car",     "Car",     "Toggle ball redirecting towards car");
  // clang-format on
  /* END THIRD LINE */

  /* FOURTH LINE */
  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();
  /* END FOURTH LINE */

  /* LEFT WINDOW */
  ImGui::BeginChild("ChildL",
                    ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, ImGui::GetContentRegionAvail().y),
                    false,
                    ImGuiWindowFlags_HorizontalScrollbar);
  if (ImGui::CollapsingHeader("Towards Goal Settings")) {
    settings_ids[0] = ImGui::GetItemID();
    addCheckbox("readirects_goal_alternating",
                "Target Alternating Goals",
                "Instead of targetting goal in front of car, they alternate");
    addRangeSliderInt("goal_shotspeed", "Towards Goal Shot Speed");
    addRangeSliderInt("goal_sideoffset", "Goal Side Offset");
    addRangeSliderInt("goal_heightoffset", "Goal Height Offset");
    addRangeSliderInt("goal_addedspin", "Goal Added Spin");

    CVarWrapper towards_goal_amount = cvarManager->getCvar("readirects_towards_goal_amount");
    if (!towards_goal_amount)
      return;
    int amt = towards_goal_amount.getIntValue();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::SliderInt("##AmtTimesTowardsGoal", &amt, 1, 10, "Amount of times ball directs towards goal in playlist: %d");
    towards_goal_amount.setValue(amt);
  } else {
    settings_ids[0] = ImGui::GetItemID();
  }

  if (ImGui::CollapsingHeader("Towards Wall Settings")) {
    settings_ids[1] = ImGui::GetItemID();
    addCheckbox(
      "readirects_wall_alternating", "Target Alternating Walls", "Instead of targetting nearest wall, they alternate");
    addRangeSliderInt("wall_shotspeed", "Towards Wall Shot Speed");
    addRangeSliderInt("wall_sideoffset", "Wall Side Offset");
    addRangeSliderInt("wall_heightoffset", "Wall Height Offset");
    addRangeSliderInt("wall_addedspin", "Wall Added Spin");

    CVarWrapper towards_wall_amount = cvarManager->getCvar("readirects_towards_wall_amount");
    if (!towards_wall_amount)
      return;
    int amt = towards_wall_amount.getIntValue();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::SliderInt("##AmtTimesTowardsWall", &amt, 1, 10, "Amount of times ball directs towards wall in playlist: %d");
    towards_wall_amount.setValue(amt);
  } else {
    settings_ids[1] = ImGui::GetItemID();
  }

  if (ImGui::CollapsingHeader("Towards Corner Settings")) {
    settings_ids[2] = ImGui::GetItemID();
    addRangeSliderInt("corner_shotspeed", "Towards Corner Shot Speed");
    addRangeSliderInt("corner_sideoffset", "Corner Side Offset");
    addRangeSliderInt("corner_heightoffset", "Corner Height Offset");
    addRangeSliderInt("corner_addedspin", "Corner Added Spin");

    CVarWrapper towards_corner_amount = cvarManager->getCvar("readirects_towards_corner_amount");
    if (!towards_corner_amount)
      return;
    int amt = towards_corner_amount.getIntValue();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::SliderInt(
      "##AmtTimesTowardsCorner", &amt, 1, 10, "Amount of times ball directs towards corner in playlist: %d");
    towards_corner_amount.setValue(amt);
  } else {
    settings_ids[2] = ImGui::GetItemID();
  }

  if (ImGui::CollapsingHeader("Towards Ceiling Settings")) {
    settings_ids[3] = ImGui::GetItemID();
    addRangeSliderInt("ceiling_shotspeed", "Towards Ceiling Shot Speed");
    addRangeSliderInt("ceiling_sideoffset", "Ceiling Side Offset");
    addRangeSliderInt("ceiling_heightoffset", "Ceiling Height Offset");
    addRangeSliderInt("ceiling_addedspin", "Ceiling Added Spin");

    CVarWrapper towards_ceiling_amount = cvarManager->getCvar("readirects_towards_ceiling_amount");
    if (!towards_ceiling_amount)
      return;
    int amt = towards_ceiling_amount.getIntValue();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::SliderInt(
      "##AmtTimesTowardsCeiling", &amt, 1, 10, "Amount of times ball directs towards ceiling in playlist: %d");
    towards_ceiling_amount.setValue(amt);
  } else {
    settings_ids[3] = ImGui::GetItemID();
  }
  if (ImGui::CollapsingHeader("Towards Car Settings")) {
    settings_ids[4] = ImGui::GetItemID();
    addCheckbox("readirects_car_pass_predict",
                "Enable position prediction",
                "Enables ball redirected towards your predicted position");
    addRangeSliderFloat("car_predict_multiplier_x", "Redirect predict multiplyer X");
    addRangeSliderFloat("car_predict_multiplier_y", "Redirect predict multiplyer y");
    addRangeSliderInt("car_shotspeed", "Towards Car Shot Speed");
    addRangeSliderInt("car_boundoffset", "Car Bounds Offset");
    addRangeSliderInt("car_heightoffset", "Car Height Offset");
    addCheckbox("readirects_car_pass_on_ground", "Redirect on ground", "Enable redirecting ball towards car on ground");
    addRangeSliderInt("car_addedspin", "Car Added Spin");

    CVarWrapper towards_car_amount = cvarManager->getCvar("readirects_towards_car_amount");
    if (!towards_car_amount)
      return;
    int amt = towards_car_amount.getIntValue();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::SliderInt("##AmtTimesTowardsCar", &amt, 1, 10, "Amount of times ball directs towards car in playlist: %d");
    towards_car_amount.setValue(amt);
  } else {
    settings_ids[4] = ImGui::GetItemID();
  }
  settings_storage = ImGui::GetStateStorage();

  ImGui::EndChild();
  /* END LEFT WINDOW */
  ImGui::SameLine();
  /* RIGHT WINDOW */
  ImGui::BeginChild("ChildR",
                    ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y),
                    false,
                    ImGuiWindowFlags_HorizontalScrollbar);
  addCheckbox("readirects_enable_timer", "Enable timer? ", "Redirects the ball based on a timer");
  ImGui::SameLine();
  CVarWrapper timer_seconds = cvarManager->getCvar("readirects_timer_seconds");
  if (!timer_seconds)
    return;
  int timer_value = timer_seconds.getIntValue();
  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
  ImGui::SliderInt("##Timer Seconds", &timer_value, 1, 60, "%d second(s) between redirects");
  timer_seconds.setValue(timer_value);

  addCheckbox("readirects_enable_afternumballhits",
              "Enable after # ball hits? ",
              "Redirects the ball after a # of times car hits ball");
  ImGui::SameLine();
  CVarWrapper ball_hits = cvarManager->getCvar("readirects_numballhitscar");
  if (!ball_hits)
    return;
  int ballhitcar_value = ball_hits.getIntValue();
  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
  ImGui::SliderInt("##Car Hits", &ballhitcar_value, 1, 10, "%d Ball Hit(s)");
  ball_hits.setValue(ballhitcar_value);

  addCheckbox("readirects_enable_afterballhitsground",
              "Enable after ball hits ground # times? ",
              "Redirects the ball when ball hits ground # times");
  ImGui::SameLine();
  CVarWrapper ground_hits = cvarManager->getCvar("readirects_numballhitsground");
  if (!ground_hits)
    return;
  int ballhitground_value = ground_hits.getIntValue();
  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
  ImGui::SliderInt("##Ground Hits", &ballhitground_value, 1, 25, "%d Ground Hit(s)");
  ground_hits.setValue(ballhitground_value);

  ImGui::AlignTextToFramePadding();
  ImGui::Text("Bind exec playlist to button: ");
  ImGui::SameLine();
  CVarWrapper btn = cvarManager->getCvar("readirects_shoot_key");
  if (!btn)
    return;
  char btnBoundText[300];
  std::snprintf(btnBoundText, 200, "%s (bind readirects_shoot in bindings)", btn.getStringValue().c_str());
  if (ImGui::Button(btnBoundText, ImVec2(ImGui::GetContentRegionAvail().x - 50, 20))) {
  }

  ImGui::Spacing();
  ImGui::Spacing();
  ImGui::AlignTextToFramePadding();
  ImGui::TextUnformatted("Playlist");
  ImGui::SameLine(100);
  addCheckbox("readirects_enable_randomizeplaylist",
              "Randomize playlist?",
              "Actions in the playlist will be executed randomly instead of in-order");
  ImGui::BeginChild(

    "ChildR_Playlist",
    ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y),
    true,
    ImGuiWindowFlags_HorizontalScrollbar);

  for (std::size_t n = 0; n < playlist.size(); ++n) {
    const char * item = playlist.at(n).c_str();
    ImGui::Selectable(item);
    if (ImGui::IsItemActive() && !ImGui::IsItemHovered()) {
      int n_next = n + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
      if (n_next >= 0 && n_next < playlist.size()) {
        playlist[n]      = playlist[n_next];
        playlist[n_next] = item;
        ImGui::ResetMouseDragDelta();
      }
    }
  }
  ImGui::EndChild();
  ImGui::EndChild();
  /* END RIGHT WINDOW */
}
