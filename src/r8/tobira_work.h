#ifndef TOBIRA_WORK_H
#define TOBIRA_WORK_H

#include "../types.h"

/* Work area of the door actor (actno 41 in the R8 act_tbl). Whoever unlocks a
   door must write trigger_open here: m_closed() consumes it and opens. */
typedef struct {
    Uint16 parent_index;
    Uint16 slave_index;
    Sint16 base_y;
    Sint16 closed_y;
    Sint16 open_y;
    Uint8 trigger_open;
} tobira8_work;

static inline tobira8_work *tobira8_get_work(sprite_status *actionwk) {
    return (tobira8_work *)actionwk->actfree;
}

#endif
