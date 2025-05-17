#include "update_policy.h"
#include "physics/physic.h"
#include "core/camera.h" // ou defina CameraBounds aqui mesmo
#include "xtypes.h"

bool should_update(Vect2D_s16* globalPosition, AABB* aabb, u8 mode){
    const AABB* cam = camera_getScreenBounds();
    const AABB* camExpanded = camera_getExpandedBounds();
    const AABB bounds = newAABB(
        globalPosition->x + aabb->min.x,
        globalPosition->x + aabb->max.x,
        globalPosition->y + aabb->min.y,
        globalPosition->y + aabb->max.y
    );
    
    switch (mode) {
        case UPDATE_ALWAYS:
            return TRUE;

        case UPDATE_VISIBLE_ONLY:
            return aabb_intersect(&bounds, cam);

        case UPDATE_NEAR_CAMERA:
            return aabb_intersect(&bounds, camExpanded);

        case UPDATE_DISABLED:
        default:
            return FALSE;
    }
}
