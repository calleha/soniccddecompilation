#ifndef PLS_WORK_H
#define PLS_WORK_H

#include "../types.h"

/* Work area of the special stage player (actno 1 in the special act table).
   The UFO code hands out a speed boost by setting speed_boost_timer - the
   reference writes ((short *)&actwk[0])[45], which is this field - and
   speed_ud() consumes it. */
typedef struct {
    Sint16 timer;
    Uint16 angle_x;
    union {
        Sint32 angle_z_accum;
        struct {
            Uint16 angle_z_fraction;
            Uint16 angle_z;
        };
    };
    Uint8 facing_frame;
    Uint8 state_timer;
    Sint16 speed_boost_timer;
} special_player_work;

static inline special_player_work *special_player_get_work(
    sprite_status_sp *plwk) {
    return (special_player_work *)plwk->actfree;
}

#endif
