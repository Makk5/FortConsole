#include <windows.h>
#include <iostream>
#include "enums.h"
#include "structs.h"
#include "util.h"

UEngine** GEngine;

typedef UObject* (__fastcall* o_StaticConstructObject_Internal)(
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
static o_StaticConstructObject_Internal StaticConstructObject_Internal;

DWORD WINAPI MainThread(LPVOID param)
{
    uint64_t base_address = reinterpret_cast<uint64_t>(GetModuleHandle(NULL));

    uint64_t GEngineAddr = reinterpret_cast<uint64_t>(Util::FindPattern
    (
        "\x48\x8B\xD3\x00\x00\x00\x00\x00\x48\x8B\x4C\x24\x40\x48\x89\x05\x00\x00\x00\x00\x48\x85\xC9", 
        "xxx?????xxxxxxxx????xxx"
    ));
    GEngine = (UEngine**)(GEngineAddr + 20 + *(int32_t*)(GEngineAddr + 16));

    uint64_t SCOIAddr = reinterpret_cast<uint64_t>(Util::FindPattern(
        "\x4C\x89\x44\x24\x18\x55\x53\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xAC\x24\x00\x00\x00\x00\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4", 
        "xxxxxxxxxxxxxxxxxxxxx????xxx????xxx????xxx"
    ));
    StaticConstructObject_Internal = (o_StaticConstructObject_Internal)(SCOIAddr);

    UConsole* Console = reinterpret_cast<UConsole*>(StaticConstructObject_Internal(
        (*GEngine)->ConsoleClass,
        reinterpret_cast<UObject*>((*GEngine)->GameViewportClient),
        nullptr,
        EObjectFlags::RF_NoFlags,
        EInternalObjectFlags::None,
        nullptr,
        false,
        nullptr,
        false
    ));

    (*GEngine)->GameViewportClient->ViewportConsole = Console;

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