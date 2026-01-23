#include "priority_queue.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void heapify_up(PriorityQueue* pq, size_t index) {
    while (index > 0) {
        size_t parent = (index - 1) / 2;
        if (pq->heap[parent].priority <= pq->heap[index].priority) {
            break;
        }
        PQNode temp = pq->heap[parent];
        pq->heap[parent] = pq->heap[index];
        pq->heap[index] = temp;
        index = parent;
    }
}

static void heapify_down(PriorityQueue* pq, size_t index) {
    while (1) {
        size_t left = 2 * index + 1;
        size_t right = 2 * index + 2;
        size_t smallest = index;

        if (left < pq->size && pq->heap[left].priority < pq->heap[smallest].priority) {
            smallest = left;
        }
        if (right < pq->size && pq->heap[right].priority < pq->heap[smallest].priority) {
            smallest = right;
        }

        if (smallest == index) {
            break;
        }

        PQNode temp = pq->heap[index];
        pq->heap[index] = pq->heap[smallest];
        pq->heap[smallest] = temp;
        index = smallest;
    }
}

PriorityQueue* pq_create(size_t initial_capacity) {
    PriorityQueue* pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    if (!pq) return NULL;

    pq->capacity = initial_capacity > 0 ? initial_capacity : 16;
    pq->size = 0;
    pq->heap = (PQNode*)malloc(pq->capacity * sizeof(PQNode));
    if (!pq->heap) {
        free(pq);
        return NULL;
    }

    return pq;
}

void pq_destroy(PriorityQueue* pq) {
    if (pq) {
        free(pq->heap);
        free(pq);
    }
}

int pq_add(PriorityQueue* pq, void* data, int priority) {
    if (!pq) return 0;

    if (pq->size >= pq->capacity) {
        size_t new_capacity = pq->capacity * 2;
        PQNode* new_heap = (PQNode*)realloc(pq->heap, new_capacity * sizeof(PQNode));
        if (!new_heap) return 0;
        pq->heap = new_heap;
        pq->capacity = new_capacity;
    }

    pq->heap[pq->size].data = data;
    pq->heap[pq->size].priority = priority;
    pq->size++;

    heapify_up(pq, pq->size - 1);
    return 1;
}

void* pq_remove(PriorityQueue* pq) {
    if (!pq || pq->size == 0) return NULL;

    void* result = pq->heap[0].data;
    pq->heap[0] = pq->heap[pq->size - 1];
    pq->size--;
    if (pq->size > 0) {
        heapify_down(pq, 0);
    }

    return result;
}
static size_t find_index(PriorityQueue* pq, void* data) {
    for (size_t i = 0; i < pq->size; i++) {
        if (pq->heap[i].data == data) {
            return i;
        }
    }
    return SIZE_MAX;
}

int pq_decrease_priority(PriorityQueue* pq, void* data, int new_priority) {
    if (!pq) return 0;

    size_t index = find_index(pq, data);
    if (index == SIZE_MAX) return 0;

    if (new_priority >= pq->heap[index].priority) {
        return 0;
    }

    pq->heap[index].priority = new_priority;
    heapify_up(pq, index);
    return 1;
}

int pq_set_priority(PriorityQueue* pq, void* data, int new_priority) {
    if (!pq) return 0;

    size_t index = find_index(pq, data);
    if (index == SIZE_MAX) return 0;

    int old_priority = pq->heap[index].priority;
    pq->heap[index].priority = new_priority;

    if (new_priority < old_priority) {
        heapify_up(pq, index);
    } else {
        heapify_down(pq, index);
    }
    return 1;
}

PriorityQueue* pq_build(void* data_array[], int priorities[], size_t count) {
    if (!data_array || !priorities || count == 0) return NULL;

    PriorityQueue* pq = pq_create(count);
    if (!pq) return NULL;

    for (size_t i = 0; i < count; i++) {
        pq->heap[i].data = data_array[i];
        pq->heap[i].priority = priorities[i];
        pq->size++;
    }

    for (int i = (int)count / 2 - 1; i >= 0; i--) {
        heapify_down(pq, (size_t)i);
    }

    return pq;
}
int pq_is_empty(PriorityQueue* pq) {
    return !pq || pq->size == 0;
}

size_t pq_size(PriorityQueue* pq) {
    return pq ? pq->size : 0;
}

void pq_print(PriorityQueue* pq, void (*print_func)(void*)) {
    if (!pq) {
        printf("Kolejka jest NULL\n");
        return;
    }

    printf("Kolejka priorytetowa (rozmiar: %zu):\n", pq->size);
    for (size_t i = 0; i < pq->size; i++) {
        printf("  [%zu] Priorytet: %d, Dane: ", i, pq->heap[i].priority);
        if (print_func) {
            print_func(pq->heap[i].data);
        } else {
            printf("%p", pq->heap[i].data);
        }
        printf("\n");
    }
}

