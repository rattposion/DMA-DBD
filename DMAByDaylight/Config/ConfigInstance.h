#pragma once
#include "Pch.h"
#include "PlayerConfig.h"
#include "SkillCheckConfig.h"
#include "Overlay.h"
class ConfigInstances
{
public:
	PlayerConfig Survivor = PlayerConfig(LIT("Survivor"));
	PlayerConfig Killer = PlayerConfig(LIT("Killer"));
	SkillCheckConfig SkillCheck = SkillCheckConfig(LIT("SkillCheck"));
	OverlayConfig Overlay = OverlayConfig(LIT("Overlay"));
	json ToJson()
	{
		json jsoned;
		jsoned.merge_patch(Survivor.ToJson());
		jsoned.merge_patch(Overlay.ToJson());
		jsoned.merge_patch(Killer.ToJson());
		jsoned.merge_patch(SkillCheck.ToJson());
		return jsoned;
	}

	void FromJson(json jsoned)
	{
		Survivor.FromJson(jsoned);
		Overlay.FromJson(jsoned);
		Killer.FromJson(jsoned);
		SkillCheck.FromJson(jsoned);
	}
};