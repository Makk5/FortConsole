/*
* FortConsole by Makks
* Build with debug configuration for nerd stuff.
*/

#define _CRT_SECURE_NO_WARNINGS

#include "enums.h"
#include "structs.h"
#include "memory.h"
#include <stdio.h>
#include <iostream>

HMODULE handle;

#ifdef _DEBUG
#define DEBUG_LOG(input, ...) printf(input, __VA_ARGS__);
#else
#define DEBUG_LOG(input, ...)
#endif

#define VALIDATE_ADDRESS(address, error) \
    if (!address) { \
        MessageBox(0, (LPCWSTR)error, L"FortConsole", MB_OK); \
        FreeLibraryAndExitThread((HMODULE)param, 0); \
        return 0; \
    }

UEngine* GEngine;

typedef UObject* (__fastcall* f_StaticConstructObject_Internal)(
    UClass* Class,
    UObject* InOuter,
    void* Name,
    EObjectFlags SetFlags,
    EInternalObjectFlags InternalSetFlags,
    UObject* Template,
    bool  bCopyTransientsFromClassDefaults,
    void* InstanceGraph,
    bool  bAssumeTemplateIsArchetype
);
static f_StaticConstructObject_Internal StaticConstructObject_Internal;

DWORD WINAPI MainThread(LPVOID param)
{
#ifdef _DEBUG
    AllocConsole();
#pragma warning( push )
#pragma warning( disable : 6031 )
    freopen("CONOUT$", "w", stdout);
#pragma warning( pop ) 
#endif

    handle = (HMODULE)param;

    uintptr_t GEngine_Address = Memory::FindPattern("\x48\x89\x74\x24\x20\xE8\x00\x00\x00\x00\x48\x8B\x4C\x24\x40\x48\x89\x05",
        "xxxxxx????xxxxxxxx");

    VALIDATE_ADDRESS(GEngine_Address, L"Failed to find GEngine.");

    GEngine = *(UEngine**)(GEngine_Address + 22 + *(int32_t*)(GEngine_Address + 18));

    DEBUG_LOG("GEngine: %p\n", GEngine);
    DEBUG_LOG("Console Class: %p\n", GEngine->ConsoleClass);
    DEBUG_LOG("Game Viewport Client: %p\n", GEngine->GameViewportClient);

    uintptr_t SCOI_Address = Memory::FindPattern("\x4C\x89\x44\x24\x18\x55\x53\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xAC\x24\x00\x00\x00\x00\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4",
        "xxxxxxxxxxxxxxxxxxxxx????xxx????xxx????xxx");

    VALIDATE_ADDRESS(SCOI_Address, L"Failed to find StaticConstructObject_Internal.");

    StaticConstructObject_Internal = (f_StaticConstructObject_Internal)(SCOI_Address);

    DEBUG_LOG("StaticConstructObject_Internal: %p\n", StaticConstructObject_Internal);

    UConsole* Console = reinterpret_cast<UConsole*>(StaticConstructObject_Internal(
        GEngine->ConsoleClass,
        reinterpret_cast<UObject*>(GEngine->GameViewportClient),
        nullptr,
        EObjectFlags::RF_NoFlags,
        EInternalObjectFlags::None,
        nullptr,
        false,
        nullptr,
        false
    ));

    DEBUG_LOG("Console: %p\n", Console);

    GEngine->GameViewportClient->ViewportConsole = Console;

    FreeLibraryAndExitThread((HMODULE)param, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, MainThread, hModule, 0, 0);
        break;
    }
    return TRUE;
}
