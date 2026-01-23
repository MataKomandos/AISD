#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "priority_queue.h"
#include <stdint.h>

#define MAX_CHARS 256
#define MAX_CODE_LEN 256

// Struktura węzła drzewa Huffmana
typedef struct HuffmanNode {
    unsigned char character;      // Znak (dla liści)
    int frequency;                // Częstotliwość
    struct HuffmanNode* left;     // Lewe dziecko
    struct HuffmanNode* right;    // Prawe dziecko
} HuffmanNode;

// Struktura słownika kodów
typedef struct {
    unsigned char character;
    char code[MAX_CODE_LEN];
    int code_length;
} CodeEntry;

// Funkcje drzewa Huffmana
HuffmanNode* huffman_create_node(unsigned char ch, int freq);
void huffman_destroy_tree(HuffmanNode* root);
HuffmanNode* huffman_build_tree(int frequencies[]);
void huffman_build_codes(HuffmanNode* root, char codes[][MAX_CODE_LEN], int code_lengths[], char* current_code, int depth);
void huffman_count_frequencies(const char* filename, int frequencies[]);
int huffman_compress(const char* input_file, const char* output_file);
int huffman_decompress(const char* input_file, const char* output_file);

#endif // HUFFMAN_H

