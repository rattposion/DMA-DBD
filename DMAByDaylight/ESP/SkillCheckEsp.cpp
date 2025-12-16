#include "Pch.h"
#include "SkillCheckEsp.h"
#include "Globals.h"

void DrawSkillCheck()
{
	if (!Configs.SkillCheck.Enabled)
		return;
	
	if (!EngineInstance)
		return;

	SkillCheckState state = EngineInstance->GetSkillCheck();
	if (!state.IsDisplayed)
		return;

	// Center of screen
	int centerX = Configs.Overlay.OverrideResolution ? Configs.Overlay.Width / 2 : GetSystemMetrics(SM_CXSCREEN) / 2;
	int centerY = Configs.Overlay.OverrideResolution ? Configs.Overlay.Height / 2 : GetSystemMetrics(SM_CYSCREEN) / 2;

	int width = Configs.SkillCheck.Size;
	int height = 20;
	int x = centerX - (width / 2);
	int y = centerY + 100; // Slightly below center

	// Draw Background
	FilledRectangle(x, y, width, height, Configs.SkillCheck.MainColour);
	OutlineRectangle(x, y, width, height, 1, Colour(0, 0, 0, 255));

	// Draw Success Zone
	// Assuming 0.0 to 1.0 range. If it's 0-100, we divide by 100. Usually Unreal is 0-1.
	float scale = (float)width;

	float successStart = state.Definition.SuccessZoneStart;
	float successEnd = state.Definition.SuccessZoneEnd;
	
	// Handle wrapping if needed, but usually it's linear 0-1 for the progress
	int successX = x + (int)(successStart * scale);
	int successW = (int)((successEnd - successStart) * scale);

	if (successW > 0)
	{
		FilledRectangle(successX, y, successW, height, Configs.SkillCheck.SuccessColour);
	}

	// Draw Bonus Zone
	float bonusStart = state.Definition.BonusZoneStart;
	float bonusLength = state.Definition.BonusZoneLength;
	
	int bonusX = x + (int)(bonusStart * scale);
	int bonusW = (int)(bonusLength * scale);

	if (bonusW > 0)
	{
		FilledRectangle(bonusX, y, bonusW, height, Configs.SkillCheck.BonusColour);
	}

	// Draw Cursor
	float progress = state.CurrentProgress;
	int cursorX = x + (int)(progress * scale);
	
	FilledRectangle(cursorX - 2, y - 5, 4, height + 10, Configs.SkillCheck.CursorColour);
	OutlineRectangle(cursorX - 2, y - 5, 4, height + 10, 1, Colour(0, 0, 0, 255));

    // Draw Debug Text
    // std::wstring debug = L"P: " + std::to_wstring(progress) + L" S: " + std::to_wstring(successStart) + L"-" + std::to_wstring(successEnd);
    // DrawText(x, y + 30, debug, "Verdana", 12, Colour(255, 255, 255, 255), CentreLeft);
}
