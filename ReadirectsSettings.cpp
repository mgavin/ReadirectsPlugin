#include "ReadirectsPlugin.h"
#include "imgui.h"

std::string ReadirectsPlugin::GetPluginName() {
	return "Readirects Plugin";
}

void ReadirectsPlugin::SetImGuiContext(uintptr_t ctx) {
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext *>(ctx));
}

void ReadirectsPlugin::RenderSettings() {
	ImGui::TextUnformatted("Readirects Plugin");
}