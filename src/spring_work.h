#ifndef SPRING_WORK_H
#define SPRING_WORK_H

#include "types.h"

/* Work area of the spring actor (actno 10 in every zone's act_tbl).
   Any module that spawns a spring as a child actor must configure it through
   this type, not through its own work struct, because the spring reads these
   fields itself. Layout mirrors the original: parent index, origin x, and the
   two offsets used to follow the parent. */
typedef struct {
    Sint16 jump_speed;
    Sint16 ground_y;
    Uint16 parent_index;
    Sint16 origin_x;
    Sint8 follow_x_offset;
    Sint8 follow_y_offset;
} spring_work;

static inline spring_work *spring_work_get(sprite_status *actionwk) {
    return (spring_work *)actionwk->actfree;
}

#endif
