#include "Pch.h"
#include "Engine.h"
#include "ActorEntity.h"
#include "Globals.h"
#include "Makcu.h"

Engine::Engine()
{
	// AUTO DUMP EXAMPLE (Se tiver as assinaturas):
	// uint64_t base = TargetProcess.GetBaseAddress(ProcessName);
	// uint64_t size = TargetProcess.GetBaseSize(ProcessName);
	// 
	// // GWorld Sig (Exemplo)
	// uint64_t gworld_addr = TargetProcess.SigScan(base, size, "48 8B 05 ? ? ? ? 48 3B C? 74 0F");
	// if (gworld_addr) {
	//     int32_t offset = TargetProcess.Read<int32_t>(gworld_addr + 3);
	//     GWorld = gworld_addr + offset + 7 - base; // Se GWorld for relativo a base
	//     printf("GWorld Found: %p\n", GWorld);
	// }

	GWorld = TargetProcess.Read<uint64_t>(TargetProcess.GetBaseAddress(ProcessName) + GWorld);
	printf("GWorld: %p\n", (void*)GWorld);
	PersistentLevel = TargetProcess.Read<uint64_t>(GWorld + PersistentLevel);
	printf("PersistentLevel: %p\n", (void*)PersistentLevel);
	OwningGameInstance = TargetProcess.Read<uint64_t>(GWorld + OwningGameInstance);
	printf("OwningGameInstance: %p\n", (void*)OwningGameInstance);
	LocalPlayers = TargetProcess.Read<uint64_t>(OwningGameInstance + LocalPlayers);
	printf("LocalPlayers: %p\n", (void*)LocalPlayers);
	LocalPlayers = TargetProcess.Read<uint64_t>(LocalPlayers);
	printf("LocalPlayers: %p\n", (void*)LocalPlayers);
	PlayerController = TargetProcess.Read<uint64_t>(LocalPlayers + PlayerController);
	printf("PlayerController: %p\n", (void*)PlayerController);
	AcknowledgedPawn = TargetProcess.Read<uint64_t>(PlayerController + AcknowledgedPawn);
	printf("AcknowledgedPawn: %p\n", (void*)AcknowledgedPawn);
	CameraManager = TargetProcess.Read<uint64_t>(PlayerController + CameraManager);
	printf("CameraManager: %p\n", (void*)CameraManager);
	CameraEntry = TargetProcess.Read<CameraCacheEntry>(CameraManager + CameraCachePrivateOffset);
	// CameraEntry is a struct, not a pointer. Printing address of it or removing the log.
    // Assuming we want to print the struct address in local memory or something?
    // The warning says: variadic 1 has type 'CameraCacheEntry'. %p expects void*.
    // We can't easily print the whole struct with %p.
    // Let's print the Rotation/Location or just comment it out as it seems to be debug spam.
    // Or if it was intended to be a pointer, it's not.
    // I'll just remove this line or fix it to print something valid.
    // Let's print the address of the variable.
	printf("CameraCacheEntry: %p\n", &CameraEntry);

}


void Engine::Cache()
{
	// Refresh addresses to handle map changes/respawns
	uint64_t base = TargetProcess.GetBaseAddress(ProcessName);
	uint64_t gworld_ptr = TargetProcess.Read<uint64_t>(base + 0xB99DB00);
	if (gworld_ptr) GWorld = gworld_ptr;

	PersistentLevel = TargetProcess.Read<uint64_t>(GWorld + 0x38);
	OwningGameInstance = TargetProcess.Read<uint64_t>(GWorld + 0x1e8);
	LocalPlayers = TargetProcess.Read<uint64_t>(OwningGameInstance + 0x40);
	LocalPlayers = TargetProcess.Read<uint64_t>(LocalPlayers);
	PlayerController = TargetProcess.Read<uint64_t>(LocalPlayers + 0x38);
	AcknowledgedPawn = TargetProcess.Read<uint64_t>(PlayerController + 0x350);
	CameraManager = TargetProcess.Read<uint64_t>(PlayerController + 0x360);

	static bool printed = false;
	if (!printed)
	{
		printf("[+] Updated Offsets:\n");
		printf(" > Base Address: %p\n", (void*)base);
		printf(" > GWorld: %p\n", (void*)GWorld);
		printf(" > PersistentLevel: %p\n", (void*)PersistentLevel);
		printf(" > OwningGameInstance: %p\n", (void*)OwningGameInstance);
		printf(" > LocalPlayers: %p\n", (void*)LocalPlayers);
		printf(" > PlayerController: %p\n", (void*)PlayerController);
		printf(" > AcknowledgedPawn (Self): %p\n", (void*)AcknowledgedPawn);
		printf(" > CameraManager: %p\n", (void*)CameraManager);
		printed = true;
	}

	OwningActor = TargetProcess.Read<uint64_t>(PersistentLevel + OwningActorOffset);
	MaxPacket = TargetProcess.Read<uint32_t>(PersistentLevel + MaxPacketOffset);
	if (MaxPacket > 2000)
		MaxPacket = 2000;

	printf("Actor Array: %p\n", (void*)OwningActor);
	printf("Actor Array Size: %d\n", (int)MaxPacket);

	std::vector<uint64_t> entitylist;
	entitylist.resize(MaxPacket);
	std::unique_ptr<uint64_t[]> object_raw_ptr = std::make_unique<uint64_t[]>(MaxPacket);
	TargetProcess.Read(OwningActor, object_raw_ptr.get(), MaxPacket * sizeof(uint64_t));
	for (size_t i = 0; i < MaxPacket; i++)
	{
		entitylist[i] = object_raw_ptr[i];
	}
	std::list<std::shared_ptr<ActorEntity>> actors;
	auto handle = TargetProcess.CreateScatterHandle();
	for (uint64_t address : entitylist)
	{
		uintptr_t actor = address;
		if (!actor)
			continue;
		
			std::shared_ptr<ActorEntity> entity = std::make_shared<ActorEntity>(actor, handle);
			actors.push_back(entity);
		
	}
	TargetProcess.ExecuteReadScatter(handle);
	TargetProcess.CloseScatterHandle(handle);


	handle = TargetProcess.CreateScatterHandle();
	for (std::shared_ptr<ActorEntity> entity : actors)
	{
		entity->SetUp1(handle);
	}
	TargetProcess.ExecuteReadScatter(handle);
	TargetProcess.CloseScatterHandle(handle);
	std::vector<std::shared_ptr<ActorEntity>> playerlist;
	for (std::shared_ptr<ActorEntity> entity : actors)
	{
		entity->SetUp2();
		if (entity->GetName() == LIT(L"Entity"))
			continue;
		if(entity->GetPosition() == Vector3::Zero())
						continue;
		playerlist.push_back(entity);
	}
	Actors = playerlist;
}
void Engine::UpdatePlayers()
{
	auto handle = TargetProcess.CreateScatterHandle();
	for (std::shared_ptr<ActorEntity> entity : Actors)
	{
		entity->UpdatePosition(handle);
	}
	TargetProcess.ExecuteReadScatter(handle);
	TargetProcess.CloseScatterHandle(handle);
}
void Engine::RefreshViewMatrix(VMMDLL_SCATTER_HANDLE handle)
{
	TargetProcess.AddScatterReadRequest(handle, CameraManager + CameraCachePrivateOffset,reinterpret_cast<void*>(&CameraEntry),sizeof(CameraCacheEntry));
}

CameraCacheEntry Engine::GetCameraCache()
{
	return CameraEntry;
}

std::vector<std::shared_ptr<ActorEntity>> Engine::GetActors()
{
	return Actors;
}

uint32_t Engine::GetActorSize()
{
	return MaxPacket;
}

SkillCheckState Engine::GetSkillCheck()
{
	SkillCheckState state = { 0 };
	if (!AcknowledgedPawn)
		return state;

	uint64_t interactionHandler = TargetProcess.Read<uint64_t>(AcknowledgedPawn + InteractionHandler);
	if (!interactionHandler)
		return state;

	uint64_t skillCheck = TargetProcess.Read<uint64_t>(interactionHandler + SkillCheck);
	if (!skillCheck)
		return state;

	state.IsDisplayed = TargetProcess.Read<bool>(skillCheck + SkillCheck_IsDisplayed);
	if (!state.IsDisplayed)
		return state;

	state.CurrentProgress = TargetProcess.Read<float>(skillCheck + SkillCheck_CurrentProgress);
	state.Definition = TargetProcess.Read<FSkillCheckDefinition>(skillCheck + SkillCheck_Definition);

	return state;
}

void Engine::AutoSkillCheck(bool enabled, int key, bool useMakcu, int comPort)
{
	static bool pressed = false;

	if (!enabled)
	{
		pressed = false;
		return;
	}

    if (useMakcu)
    {
        if (!MakcuInstance->IsConnected())
        {
             MakcuInstance->Connect(comPort);
        }
    }

	SkillCheckState state = GetSkillCheck();
	if (state.IsDisplayed)
	{
		bool inZone = false;
		if (state.Definition.SuccessZoneStart <= state.Definition.SuccessZoneEnd)
		{
			inZone = state.CurrentProgress >= state.Definition.SuccessZoneStart && state.CurrentProgress <= state.Definition.SuccessZoneEnd;
		}
		else
		{
			inZone = state.CurrentProgress >= state.Definition.SuccessZoneStart || state.CurrentProgress <= state.Definition.SuccessZoneEnd;
		}

		if (inZone)
		{
			if (!pressed)
			{
                // Enforcing Hardware-Only (Makcu) as requested
                if (useMakcu && MakcuInstance->IsConnected())
                {
                    MakcuInstance->PressKey(key);
                    pressed = true;
                }
                // Software input (SendInput) removed for security/safety
			}
		}
		else
		{
			pressed = false;
		}
	}
	else
	{
		pressed = false;
	}
}
