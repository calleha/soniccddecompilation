#include <SDL3/SDL.h>
#include "dll.h"

void (*game_init)(void);
Sint32(*game)(void);
void (*DLL_meminit)(char ***pBufTbl, void **pFuncTbl);
void (*DLL_memfree)(void);
void (*Title_DLLInit)(engine_dll *lpDllIn, char ***pBufTbl, void **pFuncTbl);
Sint32(*Title_DLLMain)(void);
void (*Title_DLLEnd)(void);
void (*SWdataSet)(ushort_union sw1, ushort_union sw2);
Sint32(*Get_vscroll)(void);
Sint32(*Get_scra_h_posiw)(void);
Sint32(*Get_scrb_h_posiw)(void);
Sint32(*FadeProc)(void);
void (*SetDebugFlag)(Uint32 newVal);
void (*GetRoundStr)(Uint16 StageNo, Uint8 Time_Flag, char *buf);
void (*Special_block_chg)(Uint16 *hane1, Uint16 *hane2, Uint16 *dmg1, Uint16 *dmg2);

static SDL_SharedObject *currentDll = NULL;

static void *loadFunction(SDL_SharedObject *dll, const char *name) {
    void *func = SDL_LoadFunction(dll, name);
    if (!func) {
        SDL_Log("missing export %s", name);
    }
    return func;
}

int DLL_Load(const char *path) {
    DLL_Unload();

    SDL_SharedObject *dll = SDL_LoadObject(path);
    if (!dll) {
        SDL_Log("Failed to load %s: %s", path, SDL_GetError());
        return 0;
    }

    game_init = loadFunction(dll, "game_init");
    game = loadFunction(dll, "game");
    DLL_meminit = loadFunction(dll, "DLL_meminit");
    DLL_memfree = loadFunction(dll, "DLL_memfree");
    SWdataSet = loadFunction(dll, "SWdataSet");
    Get_vscroll = loadFunction(dll, "Get_vscroll");
    Get_scra_h_posiw = loadFunction(dll, "Get_scra_h_posiw");
    Get_scrb_h_posiw = loadFunction(dll, "Get_scrb_h_posiw");
    FadeProc = loadFunction(dll, "FadeProc");
    SetDebugFlag = loadFunction(dll, "SetDebugFlag");
    GetRoundStr = loadFunction(dll, "GetRoundStr");
    Special_block_chg = loadFunction(dll, "Special_block_chg");

    if (!game_init || !game || !DLL_meminit) {
        SDL_Log("Missing essential exports!");
        SDL_UnloadObject(dll);
        return 0;
    }

    currentDll = dll;
    return 1;
}

int DLL_LoadTitle(const char *path) {
    DLL_Unload();

    SDL_SharedObject *dll = SDL_LoadObject(path);
    if (!dll) {
        SDL_Log("Failed to load %s: %s", path, SDL_GetError());
        return 0;
    }

    Title_DLLInit = loadFunction(dll, "DLLInit");
    Title_DLLMain = loadFunction(dll, "DLLMain");
    Title_DLLEnd = loadFunction(dll, "DLLEnd");
    SWdataSet = loadFunction(dll, "SWdataSet");

    if (!Title_DLLInit || !Title_DLLMain || !Title_DLLEnd || !SWdataSet) {
        SDL_Log("Missing essential title exports!");
        SDL_UnloadObject(dll);
        return 0;
    }

    currentDll = dll;
    return 1;
}

void DLL_Unload(void) {
    /* currentDll is either a game DLL or a title DLL.  A title DLL gets its
       DLLEnd run while it is still mapped, and every function pointer is
       cleared afterwards: attract mode switches between the two kinds, and
       stale pointers into an unmapped module used to crash on the next call. */
    if (Title_DLLEnd) {
        Title_DLLEnd();
    }
    if (currentDll) {
        SDL_UnloadObject(currentDll);
        currentDll = NULL;
    }

    Title_DLLInit = NULL;
    Title_DLLMain = NULL;
    Title_DLLEnd = NULL;
    game_init = NULL;
    game = NULL;
    DLL_meminit = NULL;
    DLL_memfree = NULL;
    SWdataSet = NULL;
    Get_vscroll = NULL;
    Get_scra_h_posiw = NULL;
    Get_scrb_h_posiw = NULL;
    FadeProc = NULL;
    SetDebugFlag = NULL;
    GetRoundStr = NULL;
    Special_block_chg = NULL;
}
