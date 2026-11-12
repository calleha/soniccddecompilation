#ifndef WALL4_WORK_H
#define WALL4_WORK_H

#include "../types.h"

/* Work area of the wall4 actor (actno 71 in the R4 act_tbl).  The R43 boss
   chase builds two of these as gates (make_gate() in boss_4.c) and opens them
   by setting `triggered`, exactly like the reference writes the gate's
   actfree[21]: wall4_wait() then lets the wall drop out of the way. */
typedef struct {
    Sint16 drop_timer;
    Uint16 flag_index;
    Uint8 triggered;
} wall4_work;

static inline wall4_work *wall4_get_work(sprite_status *actionwk) {
    return (wall4_work *)actionwk->actfree;
}

#endif
