#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <stddef.h>

// Struktura węzła kolejki priorytetowej
typedef struct PQNode {
    void* data;           // Wskaźnik na dane
    int priority;         // Priorytet (mniejsza wartość = wyższy priorytet)
} PQNode;

// Struktura kolejki priorytetowej (min-heap)
typedef struct PriorityQueue {
    PQNode* heap;         // Tablica węzłów
    size_t size;          // Aktualna liczba elementów
    size_t capacity;      // Pojemność tablicy
} PriorityQueue;

// Funkcje kolejki priorytetowej
PriorityQueue* pq_create(size_t initial_capacity);
void pq_destroy(PriorityQueue* pq);
int pq_add(PriorityQueue* pq, void* data, int priority);
void* pq_remove(PriorityQueue* pq);
int pq_decrease_priority(PriorityQueue* pq, void* data, int new_priority);
int pq_set_priority(PriorityQueue* pq, void* data, int new_priority);
PriorityQueue* pq_build(void* data_array[], int priorities[], size_t count);
int pq_is_empty(PriorityQueue* pq);
size_t pq_size(PriorityQueue* pq);
void pq_print(PriorityQueue* pq, void (*print_func)(void*));

#endif // PRIORITY_QUEUE_H

