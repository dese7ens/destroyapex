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

struct Aimbot {
    bool AimbotEnabled = true;
    bool NoRecoilEnabled = true;
    bool AllowTargetSwitch = true;

    bool WWhenAttack = true;
    bool WInScope = true;

    bool PredictMovement = true;
    bool PredictBulletDrop = true;

    HitboxType Hitbox = HitboxType::UpperChest;
    
    float PitchMultiplier = 20;
    float YawMultiplier = 14;

    float FinalDistance = 0;
    float FinalFOV = 0;

    float Speed = 40;
    float Smooth = 10;
    float ExtraSmooth = 250;
    float HipfireFOV = 30;
    float ZoomFOV = 10;
    float MinDistance = 1;
    float HipfireDistance = 60;
    float ZoomDistance = 160;

    XDisplay* X11Display;
    LocalPlayer* Myself;
    std::vector<Player*>* Players;

    Player* CurrentTarget = nullptr;
    bool TargetSelected = true;

    Vector2D previous_weaponPunchAngles;

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
            ImGui::Checkbox("No recoil", &NoRecoilEnabled);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Toggle the NoRecoil");
            ImGui::SameLine();
            ImGui::Checkbox("Allow target switch", &AllowTargetSwitch);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Allow switching Target when current target no longer meets requirements");
            
            ImGui::Separator();

            ImGui::Checkbox("Aim when attack", &WWhenAttack);
            ImGui::SameLine();
            ImGui::Checkbox("Aim in ADS", &WInScope);

            ImGui::Separator();

            ImGui::Checkbox("Predict Movement", &PredictMovement);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Predict target's movement");
            ImGui::SameLine();
            ImGui::Checkbox("Predict Bullet Drop", &PredictBulletDrop);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Predict weapon's bullet drop");
            
            ImGui::Separator();

            const char* HitboxTypes[] = { "Head", "Neck", "Upper Chest", "Lower Chest", "Stomach", "Hip" };
            int HitboxTypeIndex = static_cast<int>(Hitbox);
            ImGui::Combo("Hitbox Type", &HitboxTypeIndex, HitboxTypes, IM_ARRAYSIZE(HitboxTypes));
            Hitbox = static_cast<HitboxType>(HitboxTypeIndex);

            ImGui::Separator();

            ImGui::SliderFloat("Pitch multiplier", &PitchMultiplier, 1, 100, "%.0f");
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Y RCS factor");
            ImGui::SliderFloat("Yaw multiplier", &YawMultiplier, 1, 100, "%.0f");
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("X RCS factor");

            ImGui::Separator();

            ImGui::SliderFloat("Speed", &Speed, 1, 100, "%.0f");
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Speed of the Aim-Assist\nBigger = Faster");

            ImGui::Separator();

            ImGui::SliderFloat("Smooth", &Smooth, 1, 20, "%.0f");
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Smoothness for the Aim-Assist\nSmaller = Faster and vice versa");
            ImGui::SliderFloat("Extra Smooth", &ExtraSmooth, 100, 5000, "%.0f");
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Extra smoothness by calculating the distance between you and the target");

            ImGui::Separator();

            ImGui::SliderFloat("Hipfire FOV", &HipfireFOV, 1, 180, "%.0f");
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Field of View");
            ImGui::SliderFloat("Zoom FOV", &ZoomFOV, 1, 180, "%.0f");
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Field of View");

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
            Config::Aimbot::WWhenAttack = WWhenAttack;
            Config::Aimbot::WInScope = WInScope;
            Config::Aimbot::AllowTargetSwitch = AllowTargetSwitch;
            Config::Aimbot::PredictMovement = PredictMovement;
            Config::Aimbot::PredictBulletDrop = PredictBulletDrop;
            Config::Aimbot::Hitbox = static_cast<int>(Hitbox);
            Config::Aimbot::Speed = Speed;
            Config::Aimbot::Smooth = Smooth;
            Config::Aimbot::ExtraSmooth = ExtraSmooth;
            Config::Aimbot::HipfireFOV = HipfireFOV;
            Config::Aimbot::ZoomFOV = ZoomFOV;
            Config::Aimbot::MinDistance = MinDistance;
            Config::Aimbot::HipfireDistance = HipfireDistance;
            Config::Aimbot::ZoomDistance = ZoomDistance;
            Config::NoRecoil::RCSEnabled = NoRecoilEnabled;
            Config::NoRecoil::Pitch = PitchMultiplier;
            Config::NoRecoil::Yaw = YawMultiplier;
            return true;
        } catch (...) {
            return false;
        }
    }

    void Update() {
        // Are we knocked?
        if (!Myself->IsCombatReady()) { TargetSelected = false; return; }

        // If aim is disabled / target out of FOV //
        if (!AimRCS()) RCS();

        //std::cout << "X " << Myself->SelfAbsVelocity.x << std::endl;
        //std::cout << "Y " << Myself->SelfAbsVelocity.y << std::endl;
        //std::cout << "Z " << Myself->SelfAbsVelocity.z << std::endl;
    }

    bool AimRCS() {
        // Is aim enabled
        if (!AimbotEnabled) { ReleaseTarget(); return 0; }

        // Is aim key pressed
        if (
            !X11Display->KeyDown(XK_Caps_Lock) && 
            !(Myself->IsInAttack && WWhenAttack) &&
            !(Myself->IsZooming && WInScope)) { ReleaseTarget(); return 0; }

        // Get target
        Player* Target = CurrentTarget;
        if (!IsValidTarget(Target)) {
            if(TargetSelected && !AllowTargetSwitch)
                return 0;

            Target = FindBestTarget();
            if (!IsValidTarget(Target)) {
                CurrentTarget = nullptr;
                return 0;
            }
            
            CurrentTarget = Target;
            CurrentTarget->IsLockedOn = true;
            TargetSelected = true;
        }
        
        // FOV changes if we are zoomed or not
        if (Myself->IsZooming) {
            FinalFOV = ZoomFOV;
            FinalDistance = ZoomDistance;
        }
        else {
            FinalFOV = HipfireFOV;
            FinalDistance = HipfireDistance;
        }

        // Is target in FOV
        double DistanceFromCrosshair = CalculateDistanceFromCrosshair(CurrentTarget);
        if (DistanceFromCrosshair > FinalFOV || DistanceFromCrosshair == -1) {
            ReleaseTarget();
            return 0;
        }

        // No recoil calcs
        int nrPitchIncrement = 0;
        int nrYawIncrement = 0;
        controlWeapon(nrPitchIncrement, nrYawIncrement);

        // Get Target Angle
        QAngle DesiredAngles = QAngle(0, 0);
        if (!GetAngle(CurrentTarget, DesiredAngles))
            return 0;

        // Calculate Increment
        Vector2D DesiredAnglesIncrement = Vector2D(CalculatePitchIncrement(DesiredAngles), CalculateYawIncrement(DesiredAngles));

        // Calculate Smooth
        float Extra = ExtraSmooth / CurrentTarget->DistanceToLocalPlayer;
        float TotalSmooth = Smooth + Extra;

        // Aimbot calcs
        Vector2D aimbotDelta = DesiredAnglesIncrement
            .Divide(TotalSmooth)
            .Multiply(Speed);
        double aimYawIncrement = aimbotDelta.y * -1;
        double aimPitchIncrement = aimbotDelta.x;

        // Combine
        double totalPitchIncrement = aimPitchIncrement + nrPitchIncrement;
        double totalYawIncrement = aimYawIncrement + nrYawIncrement;

        // Turn into integers
        int totalPitchIncrementInt = RoundHalfEven(AL1AF0(totalPitchIncrement));
        int totalYawIncrementInt = RoundHalfEven(AL1AF0(totalYawIncrement));

        // Move Mouse
        if (totalPitchIncrementInt == 0 && totalYawIncrementInt == 0) return 0;
        X11Display->MoveMouse(totalPitchIncrementInt, totalYawIncrementInt);
        return 1;
    }

    void RCS() {
        // Calculate RCS
        int nrPitchIncrement = 0;
        int nrYawIncrement = 0;
        controlWeapon(nrPitchIncrement, nrYawIncrement);

        // MoveMouse
        if (nrPitchIncrement == 0 && nrYawIncrement == 0) return;
        X11Display->MoveMouse(nrPitchIncrement, nrYawIncrement);
    }

    void controlWeapon(int &rcsPitch, int &rcsYaw) {
        if (!NoRecoilEnabled) return;

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
        
        if (!Myself->IsInAttack) return;
        Vector2D punchAnglesDiff = Myself->PunchAnglesDifferent;
        if (punchAnglesDiff.IsZeroVector()){ 
            return;
        }
        rcsPitch = (punchAnglesDiff.x > 0)
            ? RoundHalfEven(punchAnglesDiff.x * PitchMultiplier)
            : 0;
        rcsYaw = RoundHalfEven(-punchAnglesDiff.y * YawMultiplier);
    }

    bool GetAngle(const Player* Target, QAngle& Angle) {
        const QAngle CurrentAngle = QAngle(Myself->ViewAngles.x, Myself->ViewAngles.y).fixAngle();
        if (!CurrentAngle.isValid())
            return false;

        if (!GetAngleToTarget(Target, Angle))
            return false;

        return true;
    }

    bool GetAngleToTarget(const Player* Target, QAngle& Angle) const {
        const Vector3D TargetPosition = Target->GetBonePosition(Hitbox);
        const Vector3D TargetVelocity = Target->AbsoluteVelocity;
        const Vector3D CameraPosition = Myself->CameraPosition;
        const QAngle CurrentAngle = QAngle(Myself->ViewAngles.x, Myself->ViewAngles.y).fixAngle();
        
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

    double CalculateDistanceFromCrosshair(Player* target) {
        Vector3D CameraPosition = Myself->CameraPosition;
        QAngle CurrentAngle = QAngle(Myself->ViewAngles.x, Myself->ViewAngles.y).fixAngle();

        Vector3D TargetPos = target->LocalOrigin;
        if (CameraPosition.Distance(TargetPos) <= 0.0001f)
            return -1;

        QAngle TargetAngle = Resolver::CalculateAngle(CameraPosition, TargetPos);
        if (!TargetAngle.isValid())
            return -1;
        
        return CurrentAngle.distanceTo(TargetAngle);
    }

    Player* FindBestTarget() {
        float NearestDistance = 9999;
        Player* BestTarget = nullptr;
        Vector3D CameraPosition = Myself->CameraPosition;
        QAngle CurrentAngle = QAngle(Myself->ViewAngles.x, Myself->ViewAngles.y).fixAngle();
        for (int i = 0; i < Players->size(); i++) {
            Player* p = Players->at(i);
            if (!IsValidTarget(p)) continue;

            double DistanceFromCrosshair = CalculateDistanceFromCrosshair(p);
            if (DistanceFromCrosshair > FinalFOV || DistanceFromCrosshair == -1)
                continue;

            if (DistanceFromCrosshair < NearestDistance) {
                BestTarget = p;
                NearestDistance = DistanceFromCrosshair;
            }
        }
        return BestTarget;
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
};
