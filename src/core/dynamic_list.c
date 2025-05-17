#include "dynamic_list.h"

#define INVALID_NODE 0xFFFF

void list_init(DynamicList* list, void (*freeDataFunc)(void*)) {
    // Inicializa todos os nós como livres
    for (u16 i = 0; i < MAX_LIST_NODES; i++) {
        list->nodes[i].data = NULL;
        list->nodes[i].next = INVALID_NODE;
        list->nodes[i].used = false;
    }
    
    list->head = INVALID_NODE;
    list->tail = INVALID_NODE;
    list->count = 0;
    list->firstFree = 0;
    list->freeData = freeDataFunc;
}

bool list_add(DynamicList* list, void* data) {
    if (!list || !data || list->count >= MAX_LIST_NODES) return false;

    // Pega o primeiro nó livre
    u16 newNode = list->firstFree;
    list->nodes[newNode].data = data;
    list->nodes[newNode].used = true;
    list->nodes[newNode].next = INVALID_NODE;

    // Atualiza o primeiro nó livre
    list->firstFree++;
    if (list->firstFree >= MAX_LIST_NODES) {
        // Procura o próximo nó livre
        list->firstFree = INVALID_NODE;
        for (u16 i = 0; i < MAX_LIST_NODES; i++) {
            if (!list->nodes[i].used) {
                list->firstFree = i;
                break;
            }
        }
    }

    // Se a lista está vazia
    if (list->head == INVALID_NODE) {
        list->head = newNode;
        list->tail = newNode;
    } else {
        // Adiciona ao final
        list->nodes[list->tail].next = newNode;
        list->tail = newNode;
    }

    list->count++;
    return true;
}

bool list_remove(DynamicList* list, void* data) {
    if (!list || !data || list->count == 0) return false;

    u16 current = list->head;
    u16 previous = INVALID_NODE;

    while (current != INVALID_NODE) {
        if (list->nodes[current].data == data) {
            // Se for o primeiro nó
            if (previous == INVALID_NODE) {
                list->head = list->nodes[current].next;
                if (list->head == INVALID_NODE) list->tail = INVALID_NODE;
            } else {
                list->nodes[previous].next = list->nodes[current].next;
                if (list->nodes[current].next == INVALID_NODE) {
                    list->tail = previous;
                }
            }

            // Libera os dados se necessário
            if (list->freeData) {
                list->freeData(list->nodes[current].data);
            }

            // Marca o nó como livre
            list->nodes[current].data = NULL;
            list->nodes[current].next = INVALID_NODE;
            list->nodes[current].used = false;

            // Atualiza o primeiro nó livre se necessário
            if (current < list->firstFree || list->firstFree == INVALID_NODE) {
                list->firstFree = current;
            }

            list->count--;
            return true;
        }
        previous = current;
        current = list->nodes[current].next;
    }

    return false;
}

void list_clear(DynamicList* list) {
    if (!list) return;

    u16 current = list->head;
    while (current != INVALID_NODE) {
        u16 next = list->nodes[current].next;
        
        if (list->freeData) {
            list->freeData(list->nodes[current].data);
        }
        
        list->nodes[current].data = NULL;
        list->nodes[current].next = INVALID_NODE;
        list->nodes[current].used = false;
        
        current = next;
    }

    list->head = INVALID_NODE;
    list->tail = INVALID_NODE;
    list->count = 0;
    list->firstFree = 0;
}

void list_foreach(DynamicList* list, void (*callback)(void* data, void* userData), void* userData) {
    if (!list || !callback) return;

    u16 current = list->head;
    while (current != INVALID_NODE) {
        callback(list->nodes[current].data, userData);
        current = list->nodes[current].next;
    }
}

bool list_contains(DynamicList* list, void* data) {
    if (!list || !data) return false;

    u16 current = list->head;
    while (current != INVALID_NODE) {
        if (list->nodes[current].data == data) return true;
        current = list->nodes[current].next;
    }

    return false;
}

u16 list_count(DynamicList* list) {
    return list ? list->count : 0;
} 