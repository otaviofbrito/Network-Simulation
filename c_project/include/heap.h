#ifndef MIN_HEAP_H
#define MIN_HEAP_H

#include <stdio.h>
#include <stdlib.h>

//foward declaration
typedef struct Event Event;

// Estrutura da Min Heap
typedef struct MinHeap
{
  Event *events; // Array de eventos
  int max_capacity;  // Capacidade máxima do heap
  int size;      // Tamanho atual do heap
} MinHeap;

/**
 * Cria uma Min Heap vazia com a capacidade especificada.
 * @param capacity Capacidade máxima da heap.
 * @return Ponteiro para a estrutura MinHeap criada.
 */
MinHeap *createMinHeap(int capacity);

/**
 * Insere um novo evento na Min Heap.
 * @param heap Ponteiro para a MinHeap.
 * @param event Evento a ser inserido.
 */
void insertMinHeap(MinHeap *heap, Event event);

/**
 * Remove e retorna o evento de menor tempo da Min Heap.
 * @param heap Ponteiro para a MinHeap.
 * @return Evento de menor tempo.
 */
Event extractMin(MinHeap *heap);

/**
 * Libera a memória alocada para a Min Heap.
 * @param heap Ponteiro para a MinHeap.
 */
void freeMinHeap(MinHeap *heap);

#endif // MIN_HEAP_H
