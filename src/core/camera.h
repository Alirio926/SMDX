// camera.h
#ifndef CAMERA_H
#define CAMERA_H

#include <genesis.h>
#include "components/rigidbody_def.h"

/**
 * @brief Camera behavior modes
 * 
 * CAMERA_MODE_DRAG_PLAYER: Camera moves and drags player when reaching edge
 * CAMERA_MODE_INDEPENDENT: Camera moves independently, player stays in place
 */
typedef enum {
    CAMERA_MODE_DRAG_PLAYER,    // Camera moves and drags player when reaching edge
    CAMERA_MODE_INDEPENDENT     // Camera moves independently, player stays in place
} CameraMode;

/**
 * @brief Initializes the camera system
 * 
 * @param targetOrNull The target entity to follow (can be NULL)
 * @param deadzoneSize The size of the deadzone around the target
 */
void camera_init(RigidBody* targetOrNull, Vect2D_u16 deadzoneSize);

/**
 * @brief Sets a new target for the camera to follow
 * 
 * @param target The new target entity (can be NULL to disable following)
 */
void camera_setTarget(RigidBody* target);

/**
 * @brief Updates camera position and handles player movement
 * This should be called once per frame
 */
void camera_update();

/**
 * @brief Sets the parallax effect factors for background
 * 
 * @param fx Horizontal parallax factor (0.0 to 1.0)
 * @param fy Vertical parallax factor (0.0 to 1.0)
 */
void camera_setParallax(fix16 fx, fix16 fy);

/**
 * @brief Enables or disables parallax effect
 * 
 * @param enable TRUE to enable parallax, FALSE to disable
 */
void camera_enableParallax(bool enable);

/**
 * @brief Sets up automatic camera scrolling
 * 
 * @param velocityX Horizontal scroll speed
 * @param velocityY Vertical scroll speed
 */
void camera_setAutoScroll(fix16 velocityX, fix16 velocityY);

/**
 * @brief Stops automatic camera scrolling
 */
void camera_clearAutoScroll();

/**
 * @brief Updates the level size for camera bounds
 * 
 * @param levelSize New level size in pixels
 */
void camera_setLevelSize(Vect2D_u16 levelSize);

/**
 * @brief Binds foreground and background maps to the camera
 * 
 * @param foreground Foreground map (required)
 * @param backgroundOrNull Background map (can be NULL)
 */
void camera_bindMaps(Map* foreground, Map* backgroundOrNull);

/**
 * @brief Gets the current camera position
 * 
 * @return Vect2D_s16 Current camera position in pixels
 */
Vect2D_s16 camera_getPosition();

/**
 * @brief Sets the camera behavior mode
 * 
 * @param mode The new camera mode (CAMERA_MODE_DRAG_PLAYER or CAMERA_MODE_INDEPENDENT)
 */
void camera_setMode(CameraMode mode);

/**
 * @brief Sets the initial camera position for autoscroll mode
 * 
 * @param x Initial X position in pixels
 * @param y Initial Y position in pixels
 */
void camera_setInitialPosition(s16 x, s16 y);

/**
 * @brief Controls whether the player can move independently when in drag mode
 * 
 * @param allow TRUE to allow independent movement, FALSE to keep player within screen bounds
 */
void camera_setAllowPlayerMovement(bool allow);

bool camera_isVisible(Vect2D_s16 position);

/**
 * @brief Gets the screen bounds of the camera
 * 
 * @return AABB Screen bounds in pixels
 */
AABB* camera_getScreenBounds(void);

/**
 * @brief Gets the expanded screen bounds of the camera
 * 
 * @return AABB Expanded screen bounds in pixels
 */
AABB* camera_getExpandedBounds(void);

/**
 * @brief Gets the maximum X position of the camera
 * 
 * @return Maximum X position in pixels
 */
s16 camera_getMaxX(void);

/**
 * @brief Gets the maximum Y position of the camera
 * 
 * @return Maximum Y position in pixels
 */
s16 camera_getMaxY(void);

#endif // CAMERA_H