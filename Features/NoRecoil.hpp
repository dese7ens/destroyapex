#pragma once
#include <iostream>
#include <vector>
#include "../Core/Level.hpp"
#include "../Core/LocalPlayer.hpp"
#include "../Core/Offsets.hpp"

#include "../Math/Vector2D.hpp"

#include "../Utils/Memory.hpp"
#include "../Utils/XDisplay.hpp"
#include "../Utils/Config.hpp"

// UI //
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

struct NoRecoil {
    bool NoRecoilEnabled = true;
    
    float PitchMultiplier = 20;
    float YawMultiplier = 14;

    XDisplay* display;
    Level* level;
    LocalPlayer* localPlayer;

    Vector2D previous_weaponPunchAngles;

    NoRecoil(XDisplay* display, Level* level, LocalPlayer* localPlayer) {
        this->display = display;
        this->level = level;
        this->localPlayer = localPlayer;
    }
    void RenderUI() {
      if (ImGui::BeginTabItem("RCS", nullptr, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton | ImGuiTabItemFlags_NoReorder)) {
          ImGui::Checkbox("No recoil", &NoRecoilEnabled);
          if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
              ImGui::SetTooltip("Toggle the NoRecoil");

          ImGui::Separator();

          ImGui::SliderFloat("Pitch multiplier", &PitchMultiplier, 1, 100, "%.0f");
          if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
              ImGui::SetTooltip("Y rcs factor");
          ImGui::SliderFloat("Yaw multiplier", &YawMultiplier, 1, 100, "%.0f");
          if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
              ImGui::SetTooltip("X rcs factor");

          ImGui::EndTabItem();
        }
    }

    bool Save() {
      try {
          Config::NoRecoil::RCSEnabled = NoRecoilEnabled;
          Config::NoRecoil::Pitch = PitchMultiplier;
          Config::NoRecoil::Yaw = YawMultiplier;
          return true;
        } catch (...) {
            return false;
        }
  }

    void Update() {
        // Where the fun begins //
        controlWeapon();
    }

    void controlWeapon() {
        if (!NoRecoilEnabled)return;

        int weaponId = localPlayer->WeaponIndex;

        if (
            weaponId == 102 ||
            weaponId == 86 ||
            weaponId == 95 ||
            weaponId == 94 ||
            weaponId == 108 ||
            weaponId == 110 ||
            weaponId == 2
        ) return;
        
        if (!localPlayer->IsInAttack) return;
        Vector2D punchAnglesDiff = localPlayer->PunchAnglesDifferent;
        if (punchAnglesDiff.IsZeroVector()){ 
            //printf("IsZeroVector \n");
            return;
        }
        int pitch = (punchAnglesDiff.x > 0)
            ? roundHalfEven(punchAnglesDiff.x * PitchMultiplier)
            : 0;
        int yaw = roundHalfEven(-punchAnglesDiff.y * YawMultiplier);
        //printf("Fine \n");
        display->MoveMouse(pitch, yaw);
    }

    int roundHalfEven(float x) {
        return (x >= 0.0)
            ? static_cast<int>(std::round(x))
            : static_cast<int>(std::round(-x)) * -1;
    }
};
