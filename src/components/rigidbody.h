#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "components/rigidbody_def.h"
#include "core/update_policy.h"
 
void rigidbody_init();

/**
 * Initialize the rigidbody system
 * Sets up the pool of available bodies and resets counters
 */

/**
 * Create a new rigidbody from the pool
 * @param layer The collision layer this body belongs to
 * @param mask Which layers this body can collide with
 * @param tag Custom identifier for this body type
 * @param flags Special behavior flags (static, gravity, etc)
 * @return Pointer to the new rigidbody, or NULL if pool is full
 */
RigidBody* rigidbody_create(UpdatePolicy logicPolicy, u8 layer, u8 mask, u16 tag); // Retorna um novo corpo do pool
/**
 * Remove a rigidbody and return it to the pool
 * @param body Pointer to the rigidbody to destroy
 */
void rigidbody_destroy(RigidBody* body); // Libera o corpo de volta para o pool
/**
 * Get a rigidbody by its index
 * @param index The index of the rigidbody to retrieve
 * @return Pointer to the rigidbody at that index
 */
RigidBody* getRigidBody(u16 index);
/**
 * Update a rigidbody's physics state
 * Applies gravity and updates position based on velocity
 * @param body The rigidbody to update
 */
void rigidbody_update(RigidBody* body); // mover, aplicar gravidade, etc.

#endif
