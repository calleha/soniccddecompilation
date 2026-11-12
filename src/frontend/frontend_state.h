#pragma once
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FRONTEND_STATE_EXIT,
    FRONTEND_STATE_TITLE,
    FRONTEND_STATE_ROUND,
    FRONTEND_STATE_WARP,
    FRONTEND_STATE_SPECIAL,
    FRONTEND_STATE_TIME_ATTACK,
    FRONTEND_STATE_ATTRACT,
    FRONTEND_STATE_ENDING,
} frontend_state;

#ifdef __cplusplus
}
#endif
