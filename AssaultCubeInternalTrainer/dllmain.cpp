#include "pch.h"
#include <windows.h>
#include <iostream>
#include <TlHelp32.h>
#include "mem.h"
#include "proc.h"

//Define Vector as a 3-part function to represent Vec3 structs
struct Vector3 { 
    float x, y, z;

};

//Special padding functions to make the offsets more resilient to updates
#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

//Weapon class with a pointer to the ammo in the currentWeapon's magazine
class weapon
{
public:
    union {
        DEFINE_MEMBER_N(int*, ammoMag, 0x14);
    };
};

//Playerent class defining offsets to different variables we may use
class Playerent
{
public:
    union {
        //              Type     Name    Offset
        DEFINE_MEMBER_N(weapon*, currentWeapon, 0x374);
        DEFINE_MEMBER_N(int, playerHealth, 0xF8);
        DEFINE_MEMBER_N(int, playerArmour, 0xFC);
        DEFINE_MEMBER_N(Vector3, headPos, 0x4);
        DEFINE_MEMBER_N(Vector3, playerPos, 0x34);
        DEFINE_MEMBER_N(Vector3, headPitchYaw, 0x40);
    };

};


DWORD WINAPI HackThread(HMODULE hModule) {
    //Create a console for debugging (optional)
    //Useful for seeing if injection, writes, and freezes were successful

    AllocConsole();
    FILE* f;
    freopen_s(&f, "Conout$", "w", stdout);

    std::cout << "Look the Console Works!\n";

    //Get a HANDLE to the base address of the module, in our case, the Assault Cube executable
    uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");

    //setting all hacks to false before definition
    bool bHealth = false, bAmmo = false, bRecoil = false;

    //Hack loop, setting Hack definitions to keys
    while (true) {
        //Get key input (could I just do a switch case statement for this???)
        if (GetAsyncKeyState(VK_END) & 1) {
            break;

        }
        if (GetAsyncKeyState(VK_NUMPAD1) & 1) {
            bHealth = !bHealth;

        }
        if (GetAsyncKeyState(VK_NUMPAD2) & 1) {
            bAmmo = !bAmmo;

        }
        //Notice our Recoil function is defined inside of its own logic since we are just patching and Nopping bytes
        if (GetAsyncKeyState(VK_NUMPAD3) & 1) {
            bRecoil = !bRecoil;

            if (bRecoil) {
                //nop it
                mem::Nop((BYTE*)(moduleBase + 0x63786), 10);

            }
            else {
                //write back the original instructions
                mem::Patch((BYTE*)(moduleBase + 0x63786), (BYTE*)"\x50\x8d\x4c\x24\x1c\x51\x8b\xce\xff\xd2", 10);

            }
        }
        //Notice our health and ammo functions are defined outside of our key definitions
        //This is because we need to define the Address of the player object and dereference the values we need

        //Continuous write/Freeze
        Playerent* localPlayer = *(Playerent**)(moduleBase + 0x10f4f4);

        if (localPlayer) {
            if (bHealth) {
                localPlayer->playerHealth = 1337;
            }
            if (bAmmo) {
                *localPlayer->currentWeapon->ammoMag = 1337;
            }
        }
        Sleep(5);

    }
    //Cleanup and Eject
    fclose(f);
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}


BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));

    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}