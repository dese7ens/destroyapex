#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include "IniReader.h"

constexpr char ConfigFile[] = "config.ini";

#define WriteSection(key) \
    conf << "[" #key "]" << "\n";
#define WritePair(section, key) \
    conf << #key " = " << Config::section::key << "\n";
#define WriteSectionEnd() conf << "\n";
#define WriteComment(msg) conf << "; " << msg << '\n';

#define ReadBool(section, key) \
    Config::section::key = reader.GetBoolean(#section, #key, Config::section::key);
#define ReadFloat(section, key) \
    Config::section::key = reader.GetFloat(#section, #key, Config::section::key);
#define ReadInt(section, key) \
    Config::section::key = reader.GetInteger(#section, #key, Config::section::key);

namespace Config {
    namespace Aimbot {
        bool Enabled = true;
        bool RCSEnabled = true;

        bool WWhenAttack = true;
        bool WInScope = true;

        bool AllowTargetSwitch = true;
        bool stopOut = false;

        bool PredictMovement = true;
        bool PredictBulletDrop = true;

        int Hitbox = 1;

        float deadZone = 1;
        float Smooth = 10;
        float ExtraSmooth = 250;
        float HipfireFOV = 10;
        float ZoomFOV = 10;
        float MinDistance = 1;
        float HipfireDistance = 200;
        float ZoomDistance = 200;
    };

    namespace Glow {
        bool Enabled = true;
        bool ItemGlow = true;
        bool DrawSeer = true;
        bool VisibleOnly = true;
        float MaxDistance = 200;
        float SeerMaxDistance = 200;
    };

    namespace Triggerbot {
        bool Enabled = true;
        bool workWObind = true;
        float Hip = 50;
        float Zoom = 200;
    };

    namespace Misc {
        bool SpectatorEnabled = true;
    };
};

void UpdateConfig() {
    std::ofstream conf(ConfigFile);
    if (conf.is_open()) {
        WriteSection(Aimbot);
        WritePair(Aimbot, Enabled);
        WritePair(Aimbot, RCSEnabled);

        WritePair(Aimbot, WWhenAttack);
        WritePair(Aimbot, WInScope);

        WritePair(Aimbot, AllowTargetSwitch);
        WritePair(Aimbot, stopOut);

        WritePair(Aimbot, PredictMovement);
        WritePair(Aimbot, PredictBulletDrop);

        WritePair(Aimbot, Hitbox);

        WritePair(Aimbot, deadZone);
        WritePair(Aimbot, Smooth);
        WritePair(Aimbot, ExtraSmooth);
        WritePair(Aimbot, HipfireFOV);
        WritePair(Aimbot, ZoomFOV);
        WritePair(Aimbot, MinDistance);
        WritePair(Aimbot, HipfireDistance);
        WritePair(Aimbot, ZoomDistance);
        WriteSectionEnd();

        WriteSection(Glow);
        WritePair(Glow, Enabled);
        WritePair(Glow, ItemGlow);
        WritePair(Glow, DrawSeer);
        WritePair(Glow, VisibleOnly);
        WritePair(Glow, MaxDistance);
        WritePair(Glow, SeerMaxDistance);
        WriteSectionEnd();

        WriteSection(Triggerbot);
        WritePair(Triggerbot, Enabled);
        WritePair(Triggerbot, workWObind);
        WritePair(Triggerbot, Hip);
        WritePair(Triggerbot, Zoom);
        WriteSectionEnd();

        WriteSection(Misc);
        WritePair(Misc, SpectatorEnabled);
        WriteSectionEnd();
        conf.close();
    }
}

bool ReadConfig(const std::string &configFile) {
    INIReader reader(configFile);
    if (reader.ParseError() < 0) {
        UpdateConfig();
        return false;
    }
    
    ReadBool(Aimbot, Enabled);
    ReadBool(Aimbot, RCSEnabled);

    ReadBool(Aimbot, WWhenAttack);
    ReadBool(Aimbot, WInScope);

    ReadBool(Aimbot, AllowTargetSwitch);
    ReadBool(Aimbot, stopOut);

    ReadBool(Aimbot, PredictMovement);
    ReadBool(Aimbot, PredictBulletDrop);

    ReadInt(Aimbot, Hitbox);

    ReadFloat(Aimbot, deadZone);
    ReadFloat(Aimbot, Smooth);
    ReadFloat(Aimbot, ExtraSmooth);
    ReadFloat(Aimbot, HipfireFOV);
    ReadFloat(Aimbot, ZoomFOV);
    ReadFloat(Aimbot, MinDistance);
    ReadFloat(Aimbot, HipfireDistance);
    ReadFloat(Aimbot, ZoomDistance);

    ReadBool(Glow, Enabled);
    ReadBool(Glow, ItemGlow);
    ReadBool(Glow, DrawSeer);
    ReadBool(Glow, VisibleOnly);
    ReadFloat(Glow, MaxDistance);
    ReadFloat(Glow, SeerMaxDistance);

    ReadBool(Triggerbot, Enabled);
    ReadBool(Triggerbot, workWObind);
    ReadFloat(Triggerbot, Hip);
    ReadFloat(Triggerbot, Zoom);

    ReadBool(Misc, SpectatorEnabled);

    UpdateConfig();
    return true;
}
