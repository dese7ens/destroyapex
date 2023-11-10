#pragma once
#include <iostream>
#include <vector>
#include <set>
#include "../Core/Player.hpp"
#include "../Core/LocalPlayer.hpp"
#include "../Core/Offsets.hpp"
#include "../Utils/Memory.hpp"
#include "../Math/Vector2D.hpp"
#include "../Math/Vector3D.hpp"
#include "../Utils/XDisplay.hpp"
#include "../Utils/Conversion.hpp"
#include "../Utils/Config.hpp"

// UI //
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

struct Triggerbot {
    bool TriggerbotEnabled = true;
    bool workWObind = true;
    float TriggerbotRangeHip = 50;
    float TriggerbotRangeZoom = 200;

    float finalRange;

    std::set<int> WeaponList = { 1, 84, 95, 86, 102, 94, 104, 88, 110, 106, 108 };

    XDisplay* X11Display;
    LocalPlayer* Myself;
    std::vector<Player*>* Players;

    Triggerbot(XDisplay* X11Display, LocalPlayer* Myself, std::vector<Player*>* GamePlayers) {
        this->X11Display = X11Display;
        this->Myself = Myself;
        this->Players = GamePlayers;
    }

    void RenderUI() {
        if (ImGui::BeginTabItem("Triggerbot", nullptr, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton | ImGuiTabItemFlags_NoReorder)) {
            ImGui::Checkbox("Triggerbot", &TriggerbotEnabled);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Will automatically shoot the target\nWill only activate when your crosshair is at target whilst holding down Triggerbot key");
            ImGui::SameLine();
            ImGui::Checkbox("Work WO Bind", &workWObind);

            ImGui::Separator();

            ImGui::SliderFloat("Max Range Hip", &TriggerbotRangeHip, 0, 500, "%.0f");
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Triggerbot's hipfire max activation range.");
            ImGui::SliderFloat("Max Range Zoom", &TriggerbotRangeZoom, 0, 1000, "%.0f");
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Triggerbot's zoom max activation range.");
            ImGui::EndTabItem();
        }
    }

    bool Save() {
        try {
            Config::Triggerbot::Enabled = TriggerbotEnabled;
            Config::Triggerbot::workWObind = workWObind;
            Config::Triggerbot::Hip = TriggerbotRangeHip;
            Config::Triggerbot::Zoom = TriggerbotRangeZoom;
            return true;
        } catch (...) {
            return false;
        }
    }

    void Update() {
        if (!TriggerbotEnabled) return;
        if (!Myself->IsCombatReady()) return;

        if (WeaponList.find(Myself->WeaponIndex) == WeaponList.end()) return;

        // Range changes if we are zoomed or not
        if (Myself->IsZooming) {
            finalRange = TriggerbotRangeZoom;
        }
        else {
            finalRange = TriggerbotRangeHip;
        }

        for (int i = 0; i < Players->size(); i++) {
            Player* player = Players->at(i);
            if (!player->IsCombatReady()) continue;
            if (!player->IsHostile) continue;
            if (!player->IsAimedAt) continue;
            if (player->DistanceToLocalPlayer < Conversion::ToGameUnits(finalRange) &&
            (X11Display->KeyDown(XK_Shift_L) || workWObind)) {
                X11Display->MouseClickLeft();
                break;
            }
        }
    }
};
