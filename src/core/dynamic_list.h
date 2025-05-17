#ifndef DYNAMIC_LIST_H
#define DYNAMIC_LIST_H

#include <genesis.h>

#define MAX_LIST_NODES 64  // Ajuste conforme necessário

/**
 * @brief Nó da lista com pool estático
 */
typedef struct ListNode {
    void* data;                 // Dados genéricos
    u16 next;                   // Índice do próximo nó (0xFF = inválido)
    bool used;                  // Se o nó está em uso
} ListNode;

/**
 * @brief Estrutura que gerencia uma lista com pool estático
 */
typedef struct {
    ListNode nodes[MAX_LIST_NODES];  // Pool de nós
    u16 head;                        // Índice do primeiro nó
    u16 tail;                        // Índice do último nó
    u16 count;                       // Número de elementos
    u16 firstFree;                   // Primeiro nó livre
    void (*freeData)(void*);         // Função para liberar os dados
} DynamicList;

/**
 * @brief Inicializa uma lista
 * 
 * @param list Lista a ser inicializada
 * @param freeDataFunc Função para liberar os dados (pode ser NULL)
 */
void list_init(DynamicList* list, void (*freeDataFunc)(void*));

/**
 * @brief Adiciona um elemento ao final da lista
 * 
 * @param list Lista onde adicionar
 * @param data Dados a serem adicionados
 * @return true se adicionou com sucesso, false caso contrário
 */
bool list_add(DynamicList* list, void* data);

/**
 * @brief Remove um elemento da lista
 * 
 * @param list Lista de onde remover
 * @param data Dados a serem removidos
 * @return true se removeu com sucesso, false caso contrário
 */
bool list_remove(DynamicList* list, void* data);

/**
 * @brief Limpa toda a lista
 * 
 * @param list Lista a ser limpa
 */
void list_clear(DynamicList* list);

/**
 * @brief Itera sobre todos os elementos da lista
 * 
 * @param list Lista a ser iterada
 * @param callback Função chamada para cada elemento
 * @param userData Dados adicionais passados para o callback
 */
void list_foreach(DynamicList* list, void (*callback)(void* data, void* userData), void* userData);

/**
 * @brief Verifica se um elemento existe na lista
 * 
 * @param list Lista onde procurar
 * @param data Dados a serem procurados
 * @return true se encontrou, false caso contrário
 */
bool list_contains(DynamicList* list, void* data);

/**
 * @brief Retorna o número de elementos na lista
 * 
 * @param list Lista a ser contada
 * @return Número de elementos
 */
u16 list_count(DynamicList* list);

#endif // DYNAMIC_LIST_H 