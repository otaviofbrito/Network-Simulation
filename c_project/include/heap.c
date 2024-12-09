#include <stdio.h>
#include <stdlib.h>

#include "heap.h"
#include "event.h"


// Função para criar uma Min Heap
MinHeap* createMinHeap(int capacity) {
    MinHeap* heap = (MinHeap*)malloc(sizeof(MinHeap));
    heap->arr = (Event*)malloc(capacity * sizeof(Event));
    heap->size = 0;
    heap->capacity = capacity;
    return heap;
}

// Função para trocar dois eventos
void swap(Event* a, Event* b) {
    Event temp = *a;
    *a = *b;
    *b = temp;
}

// Função para corrigir a propriedade de heap (min-heapify)
void minHeapify(MinHeap* heap, int index) {
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    // Verifica se o filho à esquerda é menor que o nó atual
    if (left < heap->size && heap->arr[left].tempo < heap->arr[smallest].tempo)
        smallest = left;

    // Verifica se o filho à direita é menor que o menor nó atual
    if (right < heap->size && heap->arr[right].tempo < heap->arr[smallest].tempo)
        smallest = right;

    // Se o menor não for o nó atual, troca e continua corrigindo
    if (smallest != index) {
        swap(&heap->arr[index], &heap->arr[smallest]);
        minHeapify(heap, smallest);
    }
}

// Função para inserir um novo evento na Min Heap
void insertMinHeap(MinHeap* heap, Event event) {
    if (heap->size == heap->capacity) {
        printf("Heap cheia, não é possível adicionar mais elementos.\n");
        return;
    }

    // Insere o novo evento no final
    int i = heap->size++;
    heap->arr[i] = event;

    // Ajusta a propriedade da heap (subindo o elemento, se necessário)
    while (i != 0 && heap->arr[(i - 1) / 2].tempo > heap->arr[i].tempo) {
        swap(&heap->arr[i], &heap->arr[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

// Função para remover e retornar o evento de menor tempo (raiz)
Event extractMin(MinHeap* heap) {
    if (heap->size <= 0) {
        printf("Heap vazia, não é possível extrair elementos.\n");
        Event empty = { .tipo = COLETA, .tempo = -1.0 };
        return empty;
    }

    if (heap->size == 1) {
        heap->size--;
        return heap->arr[0];
    }

    // Salva o menor elemento (raiz)
    Event root = heap->arr[0];
    // Substitui a raiz pelo último elemento
    heap->arr[0] = heap->arr[--heap->size];
    // Corrige a propriedade de heap
    minHeapify(heap, 0);

    return root;
}

// Função para liberar a memória alocada para a Min Heap
void freeMinHeap(MinHeap* heap) {
    free(heap->arr);
    free(heap);
}