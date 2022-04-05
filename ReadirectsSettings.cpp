
#include "ReadirectsPlugin.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_rangeslider.h"

ImGuiStorage *			 settings_storage;
std::vector<ImGuiID> settings_ids(5);
bool								 first_opening = true;

std::string ReadirectsPlugin::GetPluginName() {
	return "Readirects Plugin";
}

void ReadirectsPlugin::SetImGuiContext(uintptr_t ctx) {
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext *>(ctx));
}

void ReadirectsPlugin::RenderSettings() {
	auto addCheckbox = [this](std::string	 cvarName,
														const char * chkboxText,
														const char * tooltip) {
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
	addCheckbox(
		"readirects_enabled", "Enable Plugin", "Toggle Readirects Plugin");
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
										ImVec2(ImGui::GetContentRegionAvail().x * 0.5f,
													 ImGui::GetContentRegionAvail().y),
										false,
										ImGuiWindowFlags_HorizontalScrollbar);
	auto addRangeSlider =
		[this](std::string cvarSuffix, const char * label, int vmin, int vmax) {
			CVarWrapper cvar_min =
				cvarManager->getCvar("readirects_" + cvarSuffix + "_min");
			CVarWrapper cvar_max =
				cvarManager->getCvar("readirects_" + cvarSuffix + "_max");
			if (!cvar_min && !cvar_max)
				return;
			int min = cvar_min.getIntValue(), max = cvar_max.getIntValue();
			ImGui::RangeSliderInt(label, &min, &max, vmin, vmax);
			cvar_min.setValue(min);
			cvar_max.setValue(max);
		};
	ImGuiTreeNodeFlags ch = ImGuiTreeNodeFlags_Framed |
													ImGuiTreeNodeFlags_NoAutoOpenOnLog |
													ImGuiTreeNodeFlags_DefaultOpen;
	if (ImGui::CollapsingHeader("Towards Goal Settings", ch)) {
		settings_ids[0] = ImGui::GetItemID();
		addRangeSlider("goal_shotspeed", "Towards Goal Shot Speed", 0, 50);
		addRangeSlider("goal_sideoffset", "Goal Side Offset", 0, 50);
		addRangeSlider("goal_heightoffset", "Goal Height Offset", 0, 50);
		addRangeSlider("goal_addedspin", "Wall Added Spin", 0, 50);
		addCheckbox("readirects_goal_alternating",
								"Target Alternating Goals",
								"Instead of targetting goal in front of car, they alternate");
	}
	if (ImGui::CollapsingHeader("Towards Wall Settings", ch)) {
		settings_ids[1] = ImGui::GetItemID();
		addRangeSlider("wall_shotspeed", "Towards Wall Shot Speed", 0, 50);
		addRangeSlider("wall_sideoffset", "Wall Side Offset", 0, 50);
		addRangeSlider("wall_heightoffset", "Wall Height Offset", 0, 50);
		addRangeSlider("wall_addedspin", "Wall Added Spin", 0, 50);
	}
	if (ImGui::CollapsingHeader("Towards Corner Settings", ch)) {
		settings_ids[2] = ImGui::GetItemID();
		addRangeSlider("corner_shotspeed", "Towards Corner Shot Speed", 0, 50);
		addRangeSlider("corner_sideoffset", "Corner Side Offset", 0, 50);
		addRangeSlider("corner_heightoffset", "Corner Height Offset", 0, 50);
		addRangeSlider("corner_addedspin", "Corner Added Spin", 0, 50);
	}
	if (ImGui::CollapsingHeader("Towards Ceiling Settings", ch)) {
		settings_ids[3] = ImGui::GetItemID();
		addRangeSlider("ceiling_shotspeed", "Towards Ceiling Shot Speed", 0, 50);
		addRangeSlider("ceiling_sideoffset", "Ceiling Side Offset", 0, 50);
		addRangeSlider("ceiling_heightoffset", "Ceiling Height Offset", 0, 50);
		addRangeSlider("ceiling_addedspin", "Ceiling Added Spin", 0, 50);
	}
	if (ImGui::CollapsingHeader("Towards Car Settings", ch)) {
		settings_ids[4] = ImGui::GetItemID();
		addRangeSlider("car_shotspeed", "Towards Car Shot Speed", 0, 50);
		addRangeSlider("car_sideoffset", "Car Side Offset", 0, 50);
		addRangeSlider("car_heightoffset", "Car Height Offset", 0, 50);
		addRangeSlider("car_addedspin", "Car Added Spin", 0, 50);
	}
	ImGui::TextUnformatted("Sample 3");

	settings_storage = ImGui::GetStateStorage();
	if (first_opening) {
		if (settings_storage != NULL) {
			for (auto id : settings_ids) {
				settings_storage->SetInt(id, 0);
			}
		}
		first_opening = false;
	}
	ImGui::EndChild();
	/* END LEFT WINDOW */
	ImGui::SameLine();
	/* RIGHT WINDOW */
	ImGui::BeginChild("ChildR",
										ImVec2(ImGui::GetContentRegionAvail().x * 0.5f,
													 ImGui::GetContentRegionAvail().y),
										false,
										ImGuiWindowFlags_HorizontalScrollbar);
	addCheckbox("readirects_enable_timer",
							"Enable timer? ",
							"Redirects the ball based on a timer");
	ImGui::TextUnformatted("Sample 5");
	ImGui::EndChild();
	/* END RIGHT WINDOW */
}