#include "pch.h"

typedef char* (__fastcall* t_get_hwid)();
t_get_hwid o_get_hwid;
bool did_init = false;
std::string rand_hwid = "";

//https://github.com/kura1337/mastermind-fortnite-cheat-source/blob/44f28678430435d2348a579b7342cb338aea638b/hooks/Helper.cpp#L6
uintptr_t PatternScan(uintptr_t moduleAdress, const char* signature)
{
    static auto patternToByte = [](const char* pattern)
    {
        auto       bytes = std::vector<int>{};
        const auto start = const_cast<char*>(pattern);
        const auto end = const_cast<char*>(pattern) + strlen(pattern);

        for (auto current = start; current < end; ++current)
        {
            if (*current == '?')
            {
                ++current;
                if (*current == '?')
                    ++current;
                bytes.push_back(-1);
            }
            else { bytes.push_back(strtoul(current, &current, 16)); }
        }
        return bytes;
    };

    const auto dosHeader = (PIMAGE_DOS_HEADER)moduleAdress;
    const auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)moduleAdress + dosHeader->e_lfanew);

    const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
    auto       patternBytes = patternToByte(signature);
    const auto scanBytes = reinterpret_cast<std::uint8_t*>(moduleAdress);

    const auto s = patternBytes.size();
    const auto d = patternBytes.data();

    for (auto i = 0ul; i < sizeOfImage - s; ++i)
    {
        bool found = true;
        for (auto j = 0ul; j < s; ++j)
        {
            if (scanBytes[i + j] != d[j] && d[j] != -1)
            {
                found = false;
                break;
            }
        }
        if (found) { return reinterpret_cast<uintptr_t>(&scanBytes[i]); }
    }
    return NULL;
}
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
    printf("client attempting to get hwid, sending fake\n");
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

    auto found_func = PatternScan((uintptr_t)GetModuleHandleA(NULL), "55 8B EC 6A ?? 68 ?? ?? ?? ?? 64 A1 ?? ?? ?? ?? 50 81 EC ?? ?? ?? ?? A1 ?? ?? ?? ?? 33 C5 89 45 ?? 50 8D 45 ?? 64 A3 ?? ?? ?? ?? 89 4D ?? 6A ??");

    printf("based in: %p func in: %p\n", (DWORD)GetModuleHandleA(NULL), found_func);

    rand_hwid = gen_random(36);

    printf("session random hwid will be %s\n", rand_hwid.c_str());

    BYTE b1 = 0, b2 = 0;

    b1 = *(BYTE*)(found_func);
    b2 = *(BYTE*)(found_func + 1);

    printf("b1: %02x %02x\n", b1, b2);

    MH_Initialize();
    MH_CreateHook((void*)(found_func), (void*)f_get_hwid, (void**)&o_get_hwid);
    MH_EnableHook((void*)(found_func));

    b1 = *(BYTE*)(found_func);
    b2 = *(BYTE*)(found_func + 1);

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

