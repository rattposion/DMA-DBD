#pragma once
// ScriptStruct CoreUObject.Vector
struct UEVector {
	double X; // 0x0(0x8)
	double Y; // 0x8(0x8)
	double Z; // 0x10(0x8)
};
// ScriptStruct CoreUObject.Rotator
struct UERotator {
	double Pitch; // 0x0(0x8)
	double Yaw; // 0x8(0x8)
	double Roll; // 0x10(0x8)
};
struct MinimalViewInfo
{
	struct UEVector Location; // 0x0(0x18)
	struct UERotator Rotation; // 0x18(0x18)
	float FOV; // 0x30(0x4)
};
struct CameraCacheEntry
{
	float Timestamp; // 0x00(0x04)
	char pad_4[0xc]; // 0x04(0x0c)
	MinimalViewInfo POV; // 0x10(0x5e0)
};

struct FSkillCheckDefinition
{
	float SuccessZoneStart; // 0x00            
	float SuccessZoneEnd; // 0x04              
	float BonusZoneLength; //0x08             
	float BonusZoneStart; //0x0C              
	float ProgressRate; // 0x10                
	float StartingTickerPosition; // 0x14      
	bool IsDeactivatedAfterResponse; //0x18   
	float WarningSoundDelay; // 0x1C           
	bool IsAudioMuted; // 0x20                 
	bool IsJittering; // 0x21                  
	bool IsOffCenter; // 0x22                  
	bool IsSuccessZoneMirrorred; // 0x23       
	bool IsInsane; // 0x24                     
	bool IsLocallyPredicted; // 0x25           
	uint8_t pad[2]; // 0x26
};

struct SkillCheckState
{
	bool IsDisplayed;
	float CurrentProgress;
	FSkillCheckDefinition Definition;
};