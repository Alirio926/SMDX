#ifndef ITEM_H
#define ITEM_H

#include "item_def.h"
#include "xtypes.h"
#include "components/entity_def.h"

Entity* entity_spawnItem(u16 index, const ItemDef* def);
void coletar_item(Entity* self); // Comportamento padrão de coleta

#endif // ITEM_H
