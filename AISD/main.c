#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "priority_queue.h"
#include "huffman.h"

void print_int(void* data) {
    if (data) {
        printf("%d", *(int*)data);
    }
}

void print_char(void* data) {
    if (data) {
        printf("'%c'", *(char*)data);
    }
}

void test_priority_queue() {
    printf("\n=== TEST KOLEJKI PRIORYTETOWEJ ===\n\n");
    
    PriorityQueue* pq = pq_create(10);
    if (!pq) {
        printf("Błąd: Nie udało się utworzyć kolejki!\n");
        return;
    }

    int choice;
    int* data_array[100];
    int data_count = 0;

    while (1) {
        printf("\n1. Dodaj element\n");
        printf("2. Usuń element (o najwyższym priorytecie)\n");
        printf("3. Zmniejsz priorytet\n");
        printf("4. Ustaw priorytet\n");
        printf("5. Zbuduj kolejkę z tablicy\n");
        printf("6. Wyświetl kolejkę\n");
        printf("7. Powrót do menu głównego\n");
        printf("Wybierz opcję: ");
        scanf("%d", &choice);

        if (choice == 1) {
            int* value = (int*)malloc(sizeof(int));
            int priority;
            printf("Podaj wartość (int): ");
            scanf("%d", value);
            printf("Podaj priorytet: ");
            scanf("%d", &priority);
            if (pq_add(pq, value, priority)) {
                printf("Element dodany pomyślnie!\n");
                data_array[data_count++] = value;
            } else {
                printf("Błąd podczas dodawania elementu!\n");
                free(value);
            }
        }
        else if (choice == 2) {
            int* removed = (int*)pq_remove(pq);
            if (removed) {
                printf("Usunięto element: %d\n", *removed);
                free(removed);
            } else {
                printf("Kolejka jest pusta!\n");
            }
        }
        else if (choice == 3) {
            int value, new_priority;
            printf("Podaj wartość elementu: ");
            scanf("%d", &value);
            printf("Podaj nowy priorytet (mniejszy): ");
            scanf("%d", &new_priority);
            
            int* found = NULL;
            for (int i = 0; i < data_count; i++) {
                if (data_array[i] && *(data_array[i]) == value) {
                    found = data_array[i];
                    break;
                }
            }
            
            if (found && pq_decrease_priority(pq, found, new_priority)) {
                printf("Priorytet zmniejszony pomyślnie!\n");
            } else {
                printf("Błąd: Nie znaleziono elementu lub nowy priorytet nie jest mniejszy!\n");
            }
        }
        else if (choice == 4) {
            int value, new_priority;
            printf("Podaj wartość elementu: ");
            scanf("%d", &value);
            printf("Podaj nowy priorytet: ");
            scanf("%d", &new_priority);
            
            int* found = NULL;
            for (int i = 0; i < data_count; i++) {
                if (data_array[i] && *(data_array[i]) == value) {
                    found = data_array[i];
                    break;
                }
            }
            
            if (found && pq_set_priority(pq, found, new_priority)) {
                printf("Priorytet ustawiony pomyślnie!\n");
            } else {
                printf("Błąd: Nie znaleziono elementu!\n");
            }
        }
        else if (choice == 5) {
            int count;
            printf("Podaj liczbę elementów: ");
            scanf("%d", &count);
            
            if (count > 0 && count <= 100) {
                void* data[100];
                int priorities[100];
                
                printf("Podaj pary (wartość priorytet):\n");
                for (int i = 0; i < count; i++) {
                    int* val = (int*)malloc(sizeof(int));
                    printf("Element %d: ", i + 1);
                    scanf("%d %d", val, &priorities[i]);
                    data[i] = val;
                }

                while (!pq_is_empty(pq)) {
                    int* ptr = (int*)pq_remove(pq);
                    free(ptr);
                }
                for (int i = 0; i < data_count; i++) {
                    if (data_array[i]) free(data_array[i]);
                }
                pq_destroy(pq);

                pq = pq_build(data, priorities, count);
                data_count = count;
                for (int i = 0; i < count; i++) {
                    data_array[i] = (int*)data[i];
                }
                
                if (pq) {
                    printf("Kolejka zbudowana pomyślnie!\n");
                } else {
                    printf("Błąd podczas budowania kolejki!\n");
                }
            } else {
                printf("Nieprawidłowa liczba elementów!\n");
            }
        }
        else if (choice == 6) {
            pq_print(pq, print_int);
        }
        else if (choice == 7) {
            break;
        }
        else {
            printf("Nieprawidłowa opcja!\n");
        }
    }

    while (!pq_is_empty(pq)) {
        int* ptr = (int*)pq_remove(pq);
        free(ptr);
    }
    for (int i = 0; i < data_count; i++) {
        if (data_array[i]) free(data_array[i]);
    }
    pq_destroy(pq);
}

int main() {
    printf("=== PROGRAM KOMPRESJI I DEKOMPRESJI HUFFMANA ===\n");

    while (1) {
        printf("\n1. Test kolejki priorytetowej\n");
        printf("2. Kompresja pliku (Huffman)\n");
        printf("3. Dekompresja pliku (Huffman)\n");
        printf("4. Wyjście\n");
        printf("Wybierz opcję: ");

        int choice;
        scanf("%d", &choice);

        if (choice == 1) {
            test_priority_queue();
        }
        else if (choice == 2) {
            char input_file[256];
            char output_file[256];
            printf("Podaj nazwę pliku wejściowego: ");
            scanf("%255s", input_file);
            printf("Podaj nazwę pliku wyjściowego: ");
            scanf("%255s", output_file);
            huffman_compress(input_file, output_file);
        }
        else if (choice == 3) {
            char input_file[256];
            char output_file[256];
            printf("Podaj nazwę pliku skompresowanego: ");
            scanf("%255s", input_file);
            printf("Podaj nazwę pliku wyjściowego (dekompresja): ");
            scanf("%255s", output_file);
            huffman_decompress(input_file, output_file);
        }
        else if (choice == 4) {
            printf("Do widzenia!\n");
            break;
        }
        else {
            printf("Nieprawidłowa opcja!\n");
        }
    }

    return 0;
}

