#include "ReadirectsPlugin.h"
#include "imgui.h"
#include "imgui_internal.h"

std::string ReadirectsPlugin::GetPluginName() {
	return "Readirects Plugin";
}

void ReadirectsPlugin::SetImGuiContext(uintptr_t ctx) {
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext *>(ctx));
}

void ReadirectsPlugin::RenderSettings() {
	/* FIRST LINE */
	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted("Readirects Plugin");
	ImGui::SameLine(300);

	// enable plugin checkbox
	CVarWrapper enableCvar = cvarManager->getCvar("readirects_enabled");
	if (!enableCvar) {
		return;
	}
	bool enabled = enableCvar.getBoolValue();
	if (ImGui::Checkbox("Enable plugin", &enabled)) {
		enableCvar.setValue(enabled);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Toggle Readirects Plugin");
	}
	/* END FIRST LINE */

	/* SECOND LINE */
	/*
	draw_list = ImGui::GetWindowDrawList();
	p         = ImGui::GetCursorScreenPos();
	draw_list->AddLine(ImVec2(p.x, p.y),
										ImVec2(p.x + ImGui::GetWindowWidth(), p.y),
										ImColor(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)),
										1.0f);
	ImGui::Spacing();
	*/
	ImGui::Separator();
	/* END SECOND LINE */

	/* THIRD LINE */
	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted("Enable ball redirecting towards: ");
	ImGui::SameLine();
	CVarWrapper towardsGoalCvar = cvarManager->getCvar("readirects_towards_goal");
	if (!towardsGoalCvar) {
		return;
	}
	bool towardsGoalEnabled = towardsGoalCvar.getBoolValue();
	if (ImGui::Checkbox("Goal", &towardsGoalEnabled)) {
		towardsGoalCvar.setValue(towardsGoalEnabled);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Toggle ball redirecting towards goal");
	}
	ImGui::SameLine();
	CVarWrapper towardsWallCvar = cvarManager->getCvar("readirects_towards_wall");
	if (!towardsWallCvar) {
		return;
	}
	bool towardsWallEnabled = towardsWallCvar.getBoolValue();
	if (ImGui::Checkbox("Wall", &towardsWallEnabled)) {
		towardsWallCvar.setValue(towardsWallEnabled);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Toggle ball redirecting towards wall");
	}
	ImGui::SameLine();
	CVarWrapper towardsCornerCvar =
		cvarManager->getCvar("readirects_towards_corner");
	if (!towardsCornerCvar) {
		return;
	}
	bool towardsCornerEnabled = towardsCornerCvar.getBoolValue();
	if (ImGui::Checkbox("Corner", &towardsCornerEnabled)) {
		towardsCornerCvar.setValue(towardsCornerEnabled);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Toggle ball redirecting towards corner");
	}
	ImGui::SameLine();
	CVarWrapper towardsCeilingCvar =
		cvarManager->getCvar("readirects_towards_ceiling");
	if (!towardsCeilingCvar) {
		return;
	}
	bool towardsCeilingEnabled = towardsCeilingCvar.getBoolValue();
	if (ImGui::Checkbox("Ceiling", &towardsCeilingEnabled)) {
		towardsCeilingCvar.setValue(towardsCeilingEnabled);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Toggle ball redirecting towards ceiling");
	}
	ImGui::SameLine();
	CVarWrapper towardsCarCvar = cvarManager->getCvar("readirects_towards_car");
	if (!towardsCarCvar) {
		return;
	}
	bool towardsCarEnabled = towardsCarCvar.getBoolValue();
	if (ImGui::Checkbox("Car", &towardsCarEnabled)) {
		towardsCarCvar.setValue(towardsCarEnabled);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Toggle ball redirecting towards car");
	}
	/* END THIRD LINE */

	/* FOURTH LINE */
	ImGui::Separator();
	/* END FOURTH LINE */
}