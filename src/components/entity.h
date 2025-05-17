#ifndef ENTITY_H
#define ENTITY_H

#include "entity_def.h"

// Evita dependencia circurlar
struct InputType;

// Inicialização do sistema de entidades
void init_entity();

// Funções de gerenciamento de entidades
Entity* entity_create(UpdatePolicy logicPolicy, UpdatePolicy drawPolicy);  // Retorna uma nova entidade do pool
void entity_destroy(Entity* entity);  // Libera a entidade de volta para o pool

Entity* getEntity(u16 index);

// Atualização e renderização de todas as entidades
void update_all_entities();
void entity_drawAll();
void entity_setVisible(Entity* e, bool visible);
void entity_getAABB(Entity* e, AABB* out);
Vect2D_s16 entity_getGlobalPosition(Entity* e);
#endif
