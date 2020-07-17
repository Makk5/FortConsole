#pragma once

//most of these are un-needed, just here for the sake of making code easier to read for others, especially UE4 enthusiasts 

struct UObject
{
    char unknown;
};

struct UClass
{
    char unknown;
};

struct UConsole
{
    char unknown;
};

struct UGameViewportClient
{
    char unknown1[0x40];
    UConsole* ViewportConsole;
};

struct UEngine
{
    char unknown1[0xF8];
    UClass* ConsoleClass;
    char unknown2[0x688];
    UGameViewportClient* GameViewportClient;
};