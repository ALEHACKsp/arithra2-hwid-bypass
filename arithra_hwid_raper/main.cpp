#include "pch.h"

typedef char* (__fastcall* t_get_hwid)();
t_get_hwid o_get_hwid;
bool did_init = false;
std::string rand_hwid = "";

std::string gen_random(const int len) 
{
    std::string tmp_s;
    static const char alphanum[] = "0123456789" "ABCDEF";
    srand((unsigned)time(NULL) * GetCurrentProcessId());
    tmp_s.reserve(len);
    for (int i = 0; i < len; ++i) tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    return tmp_s;
}

char* f_get_hwid()
{
    auto r = o_get_hwid();
    printf("client attempting to get hwid, sending crap (orig: %s)\n", r);
    return (char*)rand_hwid.c_str();
}

void start()
{
    AllocConsole();
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    SetConsoleTitleA("lel!");
    std::cout << "hooking" << std::endl;

    auto base = 0x540000;
    printf("based in: %p func in: %p\n", base, 0x143130 + base);

    rand_hwid = gen_random(36);

    printf("random hwid will be %s\n", rand_hwid.c_str());

    BYTE b1 = 0, b2 = 0;

    b1 = *(BYTE*)(0x143130+base);
    b2 = *(BYTE*)(0x143130 + base + 1);

    printf("b1: %02x %02x\n", b1, b2);

    MH_Initialize();
    MH_CreateHook((void*)(0x143130 + base), (void*)f_get_hwid, (void**)&o_get_hwid);
    MH_EnableHook((void*)(0x143130 + base));

    b1 = *(BYTE*)(0x143130 + base);
    b2 = *(BYTE*)(0x143130 + base + 1);

    printf("b1: %02x %02x\n", b1, b2);


    printf("hooked shit\n");
    did_init = true;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH && !did_init) CreateThread(0, 0, (LPTHREAD_START_ROUTINE)start, 0, 0, 0);
    return TRUE;
}

