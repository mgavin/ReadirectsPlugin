
#include "ReadirectsPlugin.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_rangeslider.h"

ImGuiStorage *			 settings_storage;
std::vector<ImGuiID> settings_ids(5);

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
	ImGuiIO & io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
}

void ReadirectsPlugin::RenderSettings() {
	auto addCheckbox = [this](std::string cvarName, const char * chkboxText, const char * tooltip) {
		// helper function to add a checkbox
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
	auto addRangeSlider = [this](std::string cvarSuffix, const char * label) {
		CVarWrapper cvar = cvarManager->getCvar("readirects_" + cvarSuffix);
		if (!cvar)
			return;
		int					min, max;
		std::string values = cvar.getStringValue();
		sscanf(values.c_str(), "(%d, %d)", &min, &max);
		ImGui::RangeSliderInt(label, &min, &max, cvar.GetMinimum(), cvar.GetMaximum());
		std::string value = "(" + std::to_string(min) + ", " + std::to_string(max) + ")";
		cvar.setValue(value);
	};
	if (ImGui::CollapsingHeader("Towards Goal Settings")) {
		addCheckbox("readirects_goal_alternating",
								"Target Alternating Goals",
								"Instead of targetting goal in front of car, they alternate");
		addRangeSlider("goal_shotspeed", "Towards Goal Shot Speed");
		addRangeSlider("goal_sideoffset", "Goal Side Offset");
		addRangeSlider("goal_heightoffset", "Goal Height Offset");
		addRangeSlider("goal_addedspin", "Goal Added Spin");
	}
	if (settings_ids[0] == 0)
		settings_ids[0] = ImGui::GetItemID();
	if (ImGui::CollapsingHeader("Towards Wall Settings")) {
		addCheckbox("readirects_wall_alternating",
								"Target Alternating Walls",
								"Instead of targetting wall in front of car, they alternate");
		addRangeSlider("wall_shotspeed", "Towards Wall Shot Speed");
		addRangeSlider("wall_sideoffset", "Wall Side Offset");
		addRangeSlider("wall_heightoffset", "Wall Height Offset");
		addRangeSlider("wall_addedspin", "Wall Added Spin");
	}
	if (settings_ids[1] == 0)
		settings_ids[1] = ImGui::GetItemID();
	if (ImGui::CollapsingHeader("Towards Corner Settings")) {
		addRangeSlider("corner_shotspeed", "Towards Corner Shot Speed");
		addRangeSlider("corner_sideoffset", "Corner Side Offset");
		addRangeSlider("corner_heightoffset", "Corner Height Offset");
		addRangeSlider("corner_addedspin", "Corner Added Spin");
	}
	if (settings_ids[2] == 0)
		settings_ids[2] = ImGui::GetItemID();
	if (ImGui::CollapsingHeader("Towards Ceiling Settings")) {
		addRangeSlider("ceiling_shotspeed", "Towards Ceiling Shot Speed");
		addRangeSlider("ceiling_sideoffset", "Ceiling Side Offset");
		addRangeSlider("ceiling_heightoffset", "Ceiling Height Offset");
		addRangeSlider("ceiling_addedspin", "Ceiling Added Spin");
	}
	if (settings_ids[3] == 0)
		settings_ids[3] = ImGui::GetItemID();
	if (ImGui::CollapsingHeader("Towards Car Settings")) {
		addRangeSlider("car_shotspeed", "Towards Car Shot Speed");
		addRangeSlider("car_sideoffset", "Car Side Offset");
		addRangeSlider("car_heightoffset", "Car Height Offset");
		addRangeSlider("car_addedspin", "Car Added Spin");
	}
	if (settings_ids[4] == 0)
		settings_ids[4] = ImGui::GetItemID();
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
	ImGui::SliderInt("##Timer Seconds", &timer_value, 1, 60, "%d seconds between redirects");
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
	std::snprintf(btnBoundText, 200, "%s (hold button then click to change)", btn.getStringValue().c_str());
	if (ImGui::Button(btnBoundText, ImVec2(ImGui::GetContentRegionAvail().x - 50, 20))) {
		for (auto key : KEY_LIST) {
			if (gameWrapper->IsKeyPressed(gameWrapper->GetFNameIndexByString(key))) {
				btn.setValue(key);
				break;
			}
		}
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
	static const char * items[] = {"action 1", "action 2", "action 3"};
	for (int n = 0; n < IM_ARRAYSIZE(items); ++n) {
		const char * item = items[n];
		ImGui::Selectable(item);
		if (ImGui::IsItemActive() && !ImGui::IsItemHovered()) {
			int n_next = n + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
			if (n_next >= 0 && n_next < IM_ARRAYSIZE(items)) {
				items[n]			= items[n_next];
				items[n_next] = item;
				ImGui::ResetMouseDragDelta();
			}
		}
	}
	ImGui::EndChild();
	ImGui::EndChild();
	/* END RIGHT WINDOW */
}
