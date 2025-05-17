#include "camera.h"
#include "engine.h"
#include "game_config.h"
#include "tools.h" // se houver clamp aqui
#include "tiled/tiled_map.h"

/**
 * @brief Camera state structure
 * Contains all camera parameters and state
 */
static struct {
    Vect2D_s16 position;        // Current camera position
    AABB deadzone;             // Deadzone around target
    Vect2D_u16 levelSize;      // Level size in pixels
    AABB screenBounds;         // 
    AABB expandedBounds;       // Expanded bounds for near camera updates
    s16 maxCameraX;
    s16 maxCameraY;
    RigidBody* target;         // Current target to follow
    fix16 parallaxX;           // Horizontal parallax factor
    fix16 parallaxY;           // Vertical parallax factor
    bool parallaxEnabled;      // Whether parallax is enabled
    fix16 autoScrollX;         // Auto-scroll horizontal speed
    fix16 autoScrollY;         // Auto-scroll vertical speed
    bool autoScrolling;        // Whether auto-scroll is active
    CameraMode mode;           // Current camera behavior mode
    bool allowPlayerMovement;  // Whether player can move independently in drag mode
} camera;

const u16 CAMERA_EXPANDED_BOUNDS = 32;
static Map* mapFG = NULL;  // Foreground map
static Map* mapBG = NULL;  // Background map

void camera_init(RigidBody* targetOrNull, Vect2D_u16 deadzoneSize) {
    camera.target = targetOrNull;
    camera.levelSize = newVector2D_u16(tiledMap_getWidth() << 4, tiledMap_getHeight() << 4);

    // Calculate deadzone bounds centered on screen
    camera.deadzone.min.x = (SCREEN_WIDTH >> 1) - (deadzoneSize.x >> 1);
    camera.deadzone.max.x = (SCREEN_WIDTH >> 1) + (deadzoneSize.x >> 1);
    camera.deadzone.min.y = (SCREEN_HEIGHT >> 1) - (deadzoneSize.y >> 1);
    camera.deadzone.max.y = (SCREEN_HEIGHT >> 1) + (deadzoneSize.y >> 1);

    // Initialize camera state
    camera.position.x = 0;
    camera.position.y = 0;
    camera.parallaxEnabled = FALSE;
    camera.parallaxX = FIX16(0.5);
    camera.parallaxY = FIX16(0.5);
    camera.autoScrolling = FALSE;
    camera.autoScrollX = FIX16(0);
    camera.autoScrollY = FIX16(0);
    camera.mode = CAMERA_MODE_DRAG_PLAYER; // Default mode
    camera.allowPlayerMovement = TRUE; // Default to allowing player movement
}

void camera_setTarget(RigidBody* target) {
    camera.target = target;
}

void camera_setMode(CameraMode mode) {
    camera.mode = mode;
}

void camera_setInitialPosition(s16 x, s16 y) {
    // Clamp the position to map boundaries
    camera.position.x = clamp(x, 0, camera.maxCameraX);
    camera.position.y = clamp(y, 0, camera.maxCameraY);
}

void camera_setAllowPlayerMovement(bool allow) {
    camera.allowPlayerMovement = allow;
}

void camera_update() {
    // Calculate screen bounds in world coordinates
    camera.screenBounds.min.x = camera.position.x;
    camera.screenBounds.max.x = camera.position.x + SCREEN_WIDTH;
    camera.screenBounds.min.y = camera.position.y;
    camera.screenBounds.max.y = camera.position.y + SCREEN_HEIGHT;
    
    // Calculate expanded bounds (32 pixels larger in each direction)
    camera.expandedBounds.min.x = camera.screenBounds.min.x - CAMERA_EXPANDED_BOUNDS;
    camera.expandedBounds.max.x = camera.screenBounds.max.x + CAMERA_EXPANDED_BOUNDS;
    camera.expandedBounds.min.y = camera.screenBounds.min.y - CAMERA_EXPANDED_BOUNDS;
    camera.expandedBounds.max.y = camera.screenBounds.max.y + CAMERA_EXPANDED_BOUNDS;
    
    if (camera.autoScrolling) {
        s16 oldX = camera.position.x;
        s16 oldY = camera.position.y;
                
        // Update camera position with limits
        camera.position.x = clamp(
            camera.position.x + F16_toRoundedInt(camera.autoScrollX),
            0,
            camera.maxCameraX
        );
        camera.position.y = clamp(
            camera.position.y + F16_toRoundedInt(camera.autoScrollY),
            0,
            camera.maxCameraY
        );
        
        // If in drag mode and we have a target, handle player position
        if (camera.mode == CAMERA_MODE_DRAG_PLAYER && camera.target != NULL) {
            s16 deltaX = camera.position.x - oldX;
            s16 deltaY = camera.position.y - oldY;
            
            // If player movement is not allowed, keep player within screen bounds
            if (!camera.allowPlayerMovement) {
                                
                // Get player bounds
                s16 playerLeft = camera.target->globalPosition.x - camera.target->centerOffset.x;
                s16 playerRight = playerLeft + (camera.target->aabb.max.x - camera.target->aabb.min.x);
                s16 playerTop = camera.target->globalPosition.y - camera.target->centerOffset.y;
                s16 playerBottom = playerTop + (camera.target->aabb.max.y - camera.target->aabb.min.y);
                
                // Correct player position if outside screen bounds
                if (playerLeft < camera.screenBounds.min.x) {
                    camera.target->globalPosition.x = camera.screenBounds.min.x + camera.target->centerOffset.x;
                } else if (playerRight > camera.screenBounds.max.x) {
                    camera.target->globalPosition.x = camera.screenBounds.max.x - (camera.target->aabb.max.x - camera.target->aabb.min.x) + camera.target->centerOffset.x;
                }
                
                if (playerTop < camera.screenBounds.min.y) {
                    camera.target->globalPosition.y = camera.screenBounds.min.y + camera.target->centerOffset.y;
                } else if (playerBottom > camera.screenBounds.max.y) {
                    camera.target->globalPosition.y = camera.screenBounds.max.y - (camera.target->aabb.max.y - camera.target->aabb.min.y) + camera.target->centerOffset.y;
                }
            }
            
            // Apply camera movement to player
            camera.target->globalPosition.x += deltaX;
            camera.target->globalPosition.y += deltaY;
        }
    } else if (camera.target != NULL) {
        // Calculate target center position
        Vect2D_s16 center = {
            camera.target->globalPosition.x + camera.target->centerOffset.x,
            camera.target->globalPosition.y + camera.target->centerOffset.y
        };

        // Update camera position to follow target within deadzone
        if (center.x > camera.position.x + camera.deadzone.max.x)
            camera.position.x = center.x - camera.deadzone.max.x;
        else if (center.x < camera.position.x + camera.deadzone.min.x)
            camera.position.x = center.x - camera.deadzone.min.x;

        if (center.y > camera.position.y + camera.deadzone.max.y)
            camera.position.y = center.y - camera.deadzone.max.y;
        else if (center.y < camera.position.y + camera.deadzone.min.y)
            camera.position.y = center.y - camera.deadzone.min.y;
    }

    // Clamp camera position to map boundaries
    camera.position.x = clamp(camera.position.x, 0, camera.maxCameraX);
    camera.position.y = clamp(camera.position.y, 0, camera.maxCameraX);

    // Update map positions
    if (mapFG)
        MAP_scrollTo(mapFG, camera.position.x, camera.position.y);

    if (camera.parallaxEnabled && mapBG) {
        MAP_scrollTo(
            mapBG,
            F16_toInt(F16_mul(FIX16(camera.position.x), camera.parallaxX)),
            F16_toInt(F16_mul(FIX16(camera.position.y), camera.parallaxY))
        );
    }
}

void camera_setParallax(fix16 fx, fix16 fy) {
    camera.parallaxX = fx;
    camera.parallaxY = fy;
}

void camera_enableParallax(bool enable) {
    camera.parallaxEnabled = enable;
}

void camera_setAutoScroll(fix16 velocityX, fix16 velocityY) {
    camera.autoScrolling = TRUE;
    camera.autoScrollX = velocityX;
    camera.autoScrollY = velocityY;
}

void camera_clearAutoScroll() {
    camera.autoScrolling = FALSE;
    camera.autoScrollX = FIX16(0);
    camera.autoScrollY = FIX16(0);
}

void camera_setLevelSize(Vect2D_u16 levelSize) {
    camera.levelSize = levelSize;
}

Vect2D_s16 camera_getPosition() {
    return camera.position;
}

void camera_bindMaps(Map* fg, Map* bg) {
    mapFG = fg;
    mapBG = bg;

    camera.maxCameraX = (tiledMap_getWidth() << 4) - SCREEN_WIDTH;
    camera.maxCameraY = (tiledMap_getHeight() << 4) - SCREEN_HEIGHT;
}

bool camera_isVisible(Vect2D_s16 position) {
    return (position.x >= camera.position.x - 32 && position.x <= camera.position.x + 352 && 
            position.y >= camera.position.y - 32 && position.y <= camera.position.y + 288);
}

AABB* camera_getScreenBounds() { return &camera.screenBounds; }
AABB* camera_getExpandedBounds() { return &camera.expandedBounds; }
s16 camera_getMaxX() { return camera.maxCameraX; }
s16 camera_getMaxY() { return camera.maxCameraY; }