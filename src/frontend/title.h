#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include "frontend_state.h"

int Title_LoadDLL(const char *path);
frontend_state Title_Run(void);
frontend_state Title_Restart(void);
frontend_state Title_VisualMode(void);
Uint32 Title_DebugFlag(void);
void Title_Unload(void);

#ifdef __cplusplus
}
#endif
