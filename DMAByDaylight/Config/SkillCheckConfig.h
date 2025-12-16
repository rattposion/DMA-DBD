#pragma once
#include "pch.h"
class SkillCheckConfig
{
	std::string ConfigName;

public:
	SkillCheckConfig(const std::string& name)
	{
		ConfigName = name;
	}
    bool Enabled = true;
    bool AutoSkillCheck = false;
    int AutoSkillCheckKey = VK_SPACE;
    bool UseMakcu = true; // Default to true for hardware-only requirement
    int ComPort = 0;
    int Size = 200; // Size of the skillcheck circle/bar
	D2D1::ColorF MainColour = Colour(255, 255, 255); // White for circle
	D2D1::ColorF SuccessColour = Colour(255, 0, 0); // Red for success zone
	D2D1::ColorF BonusColour = Colour(255, 255, 0); // Yellow for bonus zone
	D2D1::ColorF CursorColour = Colour(0, 255, 0); // Green for cursor

    void ToJsonColour(json* j, const std::string& name, D2D1::ColorF* colour)
    {
        (*j)[ConfigName][name][LIT("r")] = colour->r;
        (*j)[ConfigName][name][LIT("g")] = colour->g;
        (*j)[ConfigName][name][LIT("b")] = colour->b;
        (*j)[ConfigName][name][LIT("a")] = colour->a;

    }
    void FromJsonColour(json j, const std::string& name, D2D1::ColorF* colour)
    {
        if (j[ConfigName].contains(name))
        {
            colour->r = j[ConfigName][name][LIT("r")];
            colour->g = j[ConfigName][name][LIT("g")];
            colour->b = j[ConfigName][name][LIT("b")];
            colour->a = j[ConfigName][name][LIT("a")];
        }
    }

    json ToJson()
    {
        json j;
        j[ConfigName][LIT("Enabled")] = Enabled;
        j[ConfigName][LIT("AutoSkillCheck")] = AutoSkillCheck;
        j[ConfigName][LIT("AutoSkillCheckKey")] = AutoSkillCheckKey;
        j[ConfigName][LIT("UseMakcu")] = UseMakcu;
        j[ConfigName][LIT("ComPort")] = ComPort;
        j[ConfigName][LIT("Size")] = Size;
        ToJsonColour(&j, LIT("MainColour"), &MainColour);
        ToJsonColour(&j, LIT("SuccessColour"), &SuccessColour);
        ToJsonColour(&j, LIT("BonusColour"), &BonusColour);
        ToJsonColour(&j, LIT("CursorColour"), &CursorColour);

        return j;
    }
    void FromJson(const json& j)
    {
        if (!j.contains(ConfigName))
            return;
        if (j[ConfigName].contains(LIT("Enabled")))
            Enabled = j[ConfigName][LIT("Enabled")];
        if (j[ConfigName].contains(LIT("AutoSkillCheck")))
            AutoSkillCheck = j[ConfigName][LIT("AutoSkillCheck")];
        if (j[ConfigName].contains(LIT("AutoSkillCheckKey")))
            AutoSkillCheckKey = j[ConfigName][LIT("AutoSkillCheckKey")];
        if (j[ConfigName].contains(LIT("UseMakcu")))
            UseMakcu = j[ConfigName][LIT("UseMakcu")];
        if (j[ConfigName].contains(LIT("ComPort")))
            ComPort = j[ConfigName][LIT("ComPort")];
        if (j[ConfigName].contains(LIT("Size")))
            Size = j[ConfigName][LIT("Size")];
        FromJsonColour(j, LIT("MainColour"), &MainColour);
        FromJsonColour(j, LIT("SuccessColour"), &SuccessColour);
        FromJsonColour(j, LIT("BonusColour"), &BonusColour);
        FromJsonColour(j, LIT("CursorColour"), &CursorColour);
    }
};
