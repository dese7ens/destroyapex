#pragma once
#include <iostream>
#include <vector>
#include "../Core/Player.hpp"
#include "../Core/LocalPlayer.hpp"
#include "../Core/Offsets.hpp"

#include "../Math/Vector2D.hpp"
#include "../Math/Vector3D.hpp"
#include "../Math/QAngle.hpp"
#include "../Math/Resolver.hpp"

#include "../Utils/Memory.hpp"
#include "../Utils/XDisplay.hpp"
#include "../Utils/Conversion.hpp"
#include "../Utils/Config.hpp"
#include "../Utils/HitboxType.hpp"

// UI //
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

// Conversion
#define DEG2RAD( x  )  ( (float)(x) * (float)(M_PI / 180.f) )

struct Aimbot {
    bool AimbotEnabled = true;
    bool RecoilEnabled = true;

    bool WWhenAttack = true;
    bool WInScope = true;

    bool PredictMovement = true;
    bool PredictBulletDrop = true;

    float FinalDistance = 0;

    float Smooth = 10;
    float deadZone = 0.01;
    float ExtraSmooth = 250;
    float FOV = 10;
    float ZoomScale = 1.2;
    float MinDistance = 1;
    float HipfireDistance = 60;
    float ZoomDistance = 160;

    float PitchPower = 3;
    float YawPower = 3;

    XDisplay* X11Display;
    LocalPlayer* Myself;
    std::vector<Player*>* Players;

    Player* CurrentTarget = nullptr;
    bool TargetSelected = true;

    Aimbot(XDisplay* X11Display, LocalPlayer* Myself, std::vector<Player*>* GamePlayers) {
        this->X11Display = X11Display;
        this->Myself = Myself;
        this->Players = GamePlayers;
    }

    void RenderUI() {
        if (ImGui::BeginTabItem("Aim", nullptr, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton | ImGuiTabItemFlags_NoReorder)) {
            ImGui::Checkbox("Aim", &AimbotEnabled);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Toggle the Aim-Assist");
            ImGui::SameLine();
            ImGui::Checkbox("Recoil Control", &RecoilEnabled);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Presonal recomendation - make Flatline almost norecoil, other guns will have no recoil too");

            ImGui::Separator();

            ImGui::Checkbox("Aim when attack", &WWhenAttack);
            ImGui::SameLine();
            ImGui::Checkbox("Aim in ADS", &WInScope);

            ImGui::Separator();
            
            ImGui::SliderFloat("Pitch", &PitchPower, 1, 100, "%.0f");
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Pitch Power");
            ImGui::SliderFloat("Yaw", &YawPower, 1, 100, "%.0f");
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Yaw Power");

            ImGui::Separator();

            ImGui::Checkbox("Predict Movement", &PredictMovement);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Predict target's movement");
            ImGui::SameLine();
            ImGui::Checkbox("Predict Bullet Drop", &PredictBulletDrop);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Predict weapon's bullet drop");

            ImGui::Separator();

            ImGui::SliderFloat("Dead Zone", &deadZone, 1, 200, "%.0f");
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Aim won't work in this radius/100 to reduce shakes");

            ImGui::Separator();

            ImGui::SliderFloat("Smooth", &Smooth, 20, 200, "%.0f");
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Smoothness for the Aim-Assist\nSmaller = Faster and vice versa");
            ImGui::SliderFloat("Extra Smooth", &ExtraSmooth, 100, 5000, "%.0f");
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Extra smoothness by calculating the distance between you and the target");

            ImGui::Separator();

            ImGui::SliderFloat("FOV", &FOV, 1, 90, "%.0f");
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Field of View");
            ImGui::SliderFloat("Zoom Scale", &ZoomScale, 0, 5, "%.1f");

            ImGui::Separator();

            ImGui::SliderFloat("Hip-fire Distance", &HipfireDistance, 1, 500, "%.0f");
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Minimum distance for Aim-Assist to work");
            ImGui::SliderFloat("Zoom Distance", &ZoomDistance, 1, 500, "%.0f");
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Maximum distance for Aim-Assist to work");

            ImGui::EndTabItem();
        }
    }

    bool Save() {
        try {
            Config::Aimbot::Enabled = AimbotEnabled;
            Config::Aimbot::RecoilControl = RecoilEnabled;

            Config::Aimbot::WWhenAttack = WWhenAttack;
            Config::Aimbot::WInScope = WInScope;

            Config::Aimbot::PredictMovement = PredictMovement;
            Config::Aimbot::PredictBulletDrop = PredictBulletDrop;

            Config::Aimbot::deadZone = deadZone;
            Config::Aimbot::Smooth = Smooth;
            Config::Aimbot::ExtraSmooth = ExtraSmooth;
            Config::Aimbot::FOV = FOV;
            Config::Aimbot::ZoomScale = ZoomScale;
            Config::Aimbot::MinDistance = MinDistance;
            Config::Aimbot::HipfireDistance = HipfireDistance;
            Config::Aimbot::ZoomDistance = ZoomDistance;

            Config::Aimbot::PitchPower = PitchPower;
            Config::Aimbot::YawPower = YawPower;
            return true;
        } catch (...) {
            return false;
        }
    }

    void Update() {
        if (!Myself->IsCombatReady()) { ReleaseTarget(); return; }
        
        RCS();
        StartAiming();
    }

    void StartAiming() {
        if (!AimbotEnabled) { ReleaseTarget(); return; }

        // Distance based on scope
        if (Myself->IsZooming)
            FinalDistance = ZoomDistance;
        else FinalDistance = HipfireDistance;

        // If keys arent pressed
        if (!X11Display->KeyDown(XK_Caps_Lock) && 
            !(Myself->IsInAttack && WWhenAttack) &&
            !(Myself->IsZooming && WInScope)) { ReleaseTarget(); return; }

        // If not greande
        if (Myself->IsHoldingGrenade) { ReleaseTarget(); return; }

        // Get target
        Player* Target = CurrentTarget;
        if (!IsValidTarget(Target)) {
            Target = FindBestTarget();
            if (!IsValidTarget(Target)) {
                ReleaseTarget();
                return;
            }
            
            CurrentTarget = Target;
            CurrentTarget->IsLockedOn = true;
            TargetSelected = true;
        }

        // Get Target Angle
        QAngle DesiredAngles = QAngle(0, 0);
        if (!GetAngle(CurrentTarget, DesiredAngles))
            return;

        // Calculate Increment
        Vector2D DesiredAnglesIncrement = Vector2D(CalculatePitchIncrement(DesiredAngles), CalculateYawIncrement(DesiredAngles));
        Vector2D DesiredAnglesIncrement2 = DesiredAnglesIncrement;

        // Calculate Smooth
        float Extra = ExtraSmooth / CurrentTarget->DistanceToLocalPlayer;
        float TotalSmooth = Smooth + Extra;

        // Aimbot calcs
        Vector2D aimbotDelta = DesiredAnglesIncrement
            .Multiply(100)
            .Divide(TotalSmooth);
        double aimYawIncrement = aimbotDelta.y * -1;
        double aimPitchIncrement = aimbotDelta.x;

        // Turn into integers
        int totalPitchIncrementInt = RoundHalfEven(AL1AF0(aimPitchIncrement));
        int totalYawIncrementInt = RoundHalfEven(AL1AF0(aimYawIncrement));

        // Deadzone check
        if (fabs(DesiredAnglesIncrement2.x) < deadZone*0.01) totalPitchIncrementInt = 0;
        if (fabs(DesiredAnglesIncrement2.y) < deadZone*0.01) totalYawIncrementInt = 0;
        if (totalPitchIncrementInt == 0 && totalYawIncrementInt == 0) return;
        
        // Move Mouse
        if (totalPitchIncrementInt == 0 && totalYawIncrementInt == 0) return;
        X11Display->MoveMouse(totalPitchIncrementInt, totalYawIncrementInt);
    }

    void RCS() {
        if (!RecoilEnabled) return;

        int weaponId = Myself->WeaponIndex;

        if (
            weaponId == 102 ||
            weaponId == 86 ||
            weaponId == 95 ||
            weaponId == 94 ||
            weaponId == 108 ||
            weaponId == 110 ||
            weaponId == 2
        ) return;

        float tmp = PitchPower;
        if (weaponId == 103) tmp = PitchPower * 1.7;
        if (weaponId == 111) tmp = PitchPower * 1.2;

        if (!Myself->IsInAttack) return;

        Vector2D punchAnglesDiff = Myself->PunchAnglesDifferent;

        if (punchAnglesDiff.IsZeroVector()){ return; }

        int rcsPitch = (punchAnglesDiff.x > 0)
            ? RoundHalfEven(punchAnglesDiff.x * tmp)
            : 0;

        int rcsYaw = RoundHalfEven(-punchAnglesDiff.y * YawPower);

        X11Display->MoveMouse(rcsPitch, rcsYaw);
    }

    bool GetAngle(Player* Target, QAngle& Angle) {
        const QAngle CurrentAngle = QAngle(Myself->ViewAngles.x, Myself->ViewAngles.y).fixAngle();
        if (!CurrentAngle.isValid())
            return false;

        if (!GetAngleToTarget(Target, Angle))
            return false;

        return true;
    }

    bool GetAngleToTarget(Player* Target, QAngle& Angle) {
        Vector3D TargetPosition = Target->GetBonePosition(static_cast<HitboxType>(GetBestBone(Target)));
        Vector3D TargetVelocity = Target->AbsoluteVelocity;
        Vector3D CameraPosition = Myself->CameraPosition;
        QAngle CurrentAngle = QAngle(Myself->ViewAngles.x, Myself->ViewAngles.y).fixAngle();
        
        if (Myself->WeaponProjectileSpeed > 1.0f) {
            if (PredictBulletDrop && PredictMovement) {
                return Resolver::CalculateAimRotationNew(CameraPosition, TargetPosition, TargetVelocity, Myself->WeaponProjectileSpeed, Myself->WeaponProjectileScale, 255, Angle);
            }
            else if (PredictBulletDrop) {
                return Resolver::CalculateAimRotationNew(CameraPosition, TargetPosition, Vector3D(0, 0, 0), Myself->WeaponProjectileSpeed, Myself->WeaponProjectileScale, 255, Angle);
            }
            else if (PredictMovement) {
                return Resolver::CalculateAimRotation(CameraPosition, TargetPosition, TargetVelocity, Myself->WeaponProjectileSpeed, Angle);
            }
        }

        Angle = Resolver::CalculateAngle(CameraPosition, TargetPosition);
        return true;   
    }

    bool IsValidTarget(Player* target) {
        if (target == nullptr ||
            !target->IsValid() || 
            !target->IsCombatReady() || 
            !target->IsVisible || 
            !target->IsHostile || 
            target->Distance2DToLocalPlayer < Conversion::ToGameUnits(MinDistance) || 
            target->Distance2DToLocalPlayer > Conversion::ToGameUnits(FinalDistance))
            return false;
        return true;
    }

    float CalculatePitchIncrement(QAngle AimbotDesiredAngles) {
        float wayA = AimbotDesiredAngles.x - Myself->ViewAngles.x;
        float wayB = 180 - abs(wayA);
        if (wayA > 0 && wayB > 0)
            wayB *= -1;
        if (fabs(wayA) < fabs(wayB))
            return wayA;
        return wayB;
    }

    float CalculateYawIncrement(QAngle AimbotDesiredAngles) {
        float wayA = AimbotDesiredAngles.y - Myself->ViewAngles.y;
        float wayB = 360 - abs(wayA);
        if (wayA > 0 && wayB > 0)
            wayB *= -1;
        if (fabs(wayA) < fabs(wayB))
            return wayA;
        return wayB;
    }

    double CalculateDistanceFromCrosshair(Vector3D TargetPosition) {
        Vector3D CameraPosition = Myself->CameraPosition;
        QAngle CurrentAngle = QAngle(Myself->ViewAngles.x, Myself->ViewAngles.y).fixAngle();

        if (CameraPosition.Distance(TargetPosition) <= 0.0001f)
            return -1;

        QAngle TargetAngle = Resolver::CalculateAngle(CameraPosition, TargetPosition);
        if (!TargetAngle.isValid())
            return -1;
        
        return CurrentAngle.distanceTo(TargetAngle);
    }

    void ReleaseTarget() {
        if (CurrentTarget != nullptr && CurrentTarget->IsValid())
            CurrentTarget->IsLockedOn = false;
        
        TargetSelected = false;
        CurrentTarget = nullptr;
    }

    int RoundHalfEven(float x) {
        return (x >= 0.0)
            ? static_cast<int>(std::round(x))
            : static_cast<int>(std::round(-x)) * -1;
    }

    float AL1AF0(float num) {
        if (num > 0) return std::max(num, 1.0f);
        return std::min(num, -1.0f);
    }

    float GetFOVScale() {
        if (Myself->IsValid()) {
            if (Myself->IsZooming) {
                if (Myself->TargetZoomFOV > 1.0 && Myself->TargetZoomFOV < 90.0) {
                    return tanf(DEG2RAD(Myself->TargetZoomFOV) * (0.64285714285));
                }
            }
        }
        return 1.0;
    }

    int GetBestBone(Player* Target) {
        float NearestDistance = 999;
        int NearestBone = 2;
        for (int i = 0; i < 6; i++) {
            HitboxType Bone = static_cast<HitboxType>(i);
            double DistanceFromCrosshair = CalculateDistanceFromCrosshair(Target->GetBonePosition(Bone));
            if (DistanceFromCrosshair < NearestDistance) {
                NearestBone = i;
                NearestDistance = DistanceFromCrosshair;
            }
        }
        return NearestBone;
    }

    Player* FindBestTarget() {
        Player* NearestTarget = nullptr;

        float BestDistance = 9999;
        int BestHP = 100;
        int BestArmor = 125;
        float BestFOV = std::min(FOV, FOV * (GetFOVScale() * ZoomScale));

        Vector3D CameraPosition = Myself->CameraPosition;
        for (int i = 0; i < Players->size(); i++) {
            Player* p = Players->at(i);
            if (!IsValidTarget(p)) continue;
            
            if (p->DistanceToLocalPlayer < Conversion::ToGameUnits(ZoomDistance)) {
                HitboxType BestBone = static_cast<HitboxType>(GetBestBone(p));
                Vector3D TargetPosition = p->GetBonePosition(BestBone);

                float Distance = CameraPosition.Distance(TargetPosition);
                float FOV = CalculateDistanceFromCrosshair(TargetPosition);
                float HP = p->Health;
                float Armor = p->Shield;
                
                if (Distance > BestDistance) continue;
                if (FOV > BestFOV) continue;
                if (HP > BestHP) continue;
                if (Armor > BestArmor) continue;

                NearestTarget = p;
                BestDistance = Distance;
                BestHP = HP;
                BestArmor = Armor;
            }
        }
        return NearestTarget;
    }
};