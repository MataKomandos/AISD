#include "huffman.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

HuffmanNode* huffman_create_node(unsigned char ch, int freq) {
    HuffmanNode* node = (HuffmanNode*)malloc(sizeof(HuffmanNode));
    if (!node) return NULL;

    node->character = ch;
    node->frequency = freq;
    node->left = NULL;
    node->right = NULL;
    return node;
}

void huffman_destroy_tree(HuffmanNode* root) {
    if (!root) return;
    huffman_destroy_tree(root->left);
    huffman_destroy_tree(root->right);
    free(root);
}

HuffmanNode* huffman_build_tree(int frequencies[]) {
    PriorityQueue* pq = pq_create(MAX_CHARS);
    if (!pq) return NULL;

    for (int i = 0; i < MAX_CHARS; i++) {
        if (frequencies[i] > 0) {
            HuffmanNode* node = huffman_create_node((unsigned char)i, frequencies[i]);
            if (node) {
                pq_add(pq, node, frequencies[i]);
            }
        }
    }

    while (pq_size(pq) > 1) {
        HuffmanNode* left = (HuffmanNode*)pq_remove(pq);
        HuffmanNode* right = (HuffmanNode*)pq_remove(pq);

        HuffmanNode* merged = huffman_create_node(0, left->frequency + right->frequency);
        if (!merged) {
            huffman_destroy_tree(left);
            huffman_destroy_tree(right);
            pq_destroy(pq);
            return NULL;
        }

        merged->left = left;
        merged->right = right;
        pq_add(pq, merged, merged->frequency);
    }

    HuffmanNode* root = (HuffmanNode*)pq_remove(pq);
    pq_destroy(pq);
    return root;
}

void huffman_build_codes(HuffmanNode* root, char codes[][MAX_CODE_LEN], int code_lengths[], char* current_code, int depth) {
    if (!root) return;

    if (!root->left && !root->right) {
        if (depth < MAX_CODE_LEN - 1) {
            current_code[depth] = '\0';
            strcpy(codes[root->character], current_code);
            code_lengths[root->character] = depth;
        }
        return;
    }

    if (depth < MAX_CODE_LEN - 1) {
        current_code[depth] = '0';
        huffman_build_codes(root->left, codes, code_lengths, current_code, depth + 1);
    }

    if (depth < MAX_CODE_LEN - 1) {
        current_code[depth] = '1';
        huffman_build_codes(root->right, codes, code_lengths, current_code, depth + 1);
    }
}

void huffman_count_frequencies(const char* filename, int frequencies[]) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        for (int i = 0; i < MAX_CHARS; i++) {
            frequencies[i] = 0;
        }
        return;
    }

    for (int i = 0; i < MAX_CHARS; i++) {
        frequencies[i] = 0;
    }

    int ch;
    while ((ch = fgetc(file)) != EOF) {
        frequencies[ch]++;
    }

    fclose(file);
}

int huffman_compress(const char* input_file, const char* output_file) {
    int frequencies[MAX_CHARS];
    huffman_count_frequencies(input_file, frequencies);

    int total_chars = 0;
    for (int i = 0; i < MAX_CHARS; i++) {
        total_chars += frequencies[i];
    }
    if (total_chars == 0) {
        printf("Błąd: Plik wejściowy jest pusty!\n");
        return 0;
    }

    HuffmanNode* root = huffman_build_tree(frequencies);
    if (!root) {
        printf("Błąd: Nie udało się zbudować drzewa Huffmana!\n");
        return 0;
    }

    char codes[MAX_CHARS][MAX_CODE_LEN];
    int code_lengths[MAX_CHARS];
    char current_code[MAX_CODE_LEN];
    for (int i = 0; i < MAX_CHARS; i++) {
        codes[i][0] = '\0';
        code_lengths[i] = 0;
    }
    huffman_build_codes(root, codes, code_lengths, current_code, 0);

    FILE* input = fopen(input_file, "rb");
    FILE* output = fopen(output_file, "w");
    if (!input || !output) {
        printf("Błąd: Nie udało się otworzyć plików!\n");
        huffman_destroy_tree(root);
        if (input) fclose(input);
        if (output) fclose(output);
        return 0;
    }

    fprintf(output, "SŁOWNIK:\n");
    for (int i = 0; i < MAX_CHARS; i++) {
        if (frequencies[i] > 0) {
            if (i >= 32 && i <= 126) {
                fprintf(output, "%c: %d - %s\n", i, frequencies[i], codes[i]);
            } else if (i == ' ') {
                fprintf(output, "SPACJA: %d - %s\n", frequencies[i], codes[i]);
            } else if (i == '\n') {
                fprintf(output, "ENTER: %d - %s\n", frequencies[i], codes[i]);
            } else if (i == '\t') {
                fprintf(output, "TAB: %d - %s\n", frequencies[i], codes[i]);
            } else {
                fprintf(output, "\\x%02X: %d - %s\n", i, frequencies[i], codes[i]);
            }
        }
    }

    fprintf(output, "DANE:\n");
    
    int bit_buffer = 0;
    int bit_count = 0;
    int ch;
    
    while ((ch = fgetc(input)) != EOF) {
        const char* code = codes[ch];
        int len = code_lengths[ch];
        
        for (int i = 0; i < len; i++) {
            bit_buffer = (bit_buffer << 1) | (code[i] - '0');
            bit_count++;
            
            if (bit_count == 8) {
                fprintf(output, "%c", (unsigned char)bit_buffer);
                bit_buffer = 0;
                bit_count = 0;
            }
        }
    }

    int padding = 0;
    if (bit_count > 0) {
        padding = 8 - bit_count;
        bit_buffer <<= padding;
        fprintf(output, "%c", (unsigned char)bit_buffer);
    }
    fprintf(output, "\nPADDING: %d\n", padding);

    fclose(input);
    fclose(output);
    huffman_destroy_tree(root);

    printf("Kompresja zakończona pomyślnie!\n");
    return 1;
}

int huffman_decompress(const char* input_file, const char* output_file) {
    FILE* input = fopen(input_file, "r");
    FILE* output = fopen(output_file, "wb");
    if (!input || !output) {
        printf("Błąd: Nie udało się otworzyć plików!\n");
        if (input) fclose(input);
        if (output) fclose(output);
        return 0;
    }

    char line[1024];
    char codes[MAX_CHARS][MAX_CODE_LEN];
    int code_lengths[MAX_CHARS];
    for (int i = 0; i < MAX_CHARS; i++) {
        codes[i][0] = '\0';
        code_lengths[i] = 0;
    }

    if (!fgets(line, sizeof(line), input)) {
        fclose(input);
        fclose(output);
        return 0;
    }

    while (fgets(line, sizeof(line), input)) {
        if (strncmp(line, "DANE:", 5) == 0) {
            break;
        }

        char* colon = strchr(line, ':');
        if (!colon) continue;

        char* dash = strchr(colon + 1, '-');
        if (!dash) continue;

        unsigned char ch;
        if (strncmp(line, "SPACJA", 6) == 0) {
            ch = ' ';
        } else if (strncmp(line, "ENTER", 5) == 0) {
            ch = '\n';
        } else if (strncmp(line, "TAB", 3) == 0) {
            ch = '\t';
        } else if (line[0] == '\\' && line[1] == 'x') {
            int hex_val;
            sscanf(line, "\\x%02X", &hex_val);
            ch = (unsigned char)hex_val;
        } else {
            ch = line[0];
        }

        dash++;
        while (*dash == ' ' || *dash == '\t') dash++;
        int code_len = 0;
        while (*dash != '\n' && *dash != '\r' && *dash != '\0' && code_len < MAX_CODE_LEN - 1) {
            codes[ch][code_len++] = *dash++;
        }
        codes[ch][code_len] = '\0';
        code_lengths[ch] = code_len;
    }

    typedef struct {
        char code[MAX_CODE_LEN];
        unsigned char character;
    } ReverseCode;

    ReverseCode reverse_codes[256];
    int reverse_count = 0;
    for (int i = 0; i < MAX_CHARS; i++) {
        if (code_lengths[i] > 0) {
            strcpy(reverse_codes[reverse_count].code, codes[i]);
            reverse_codes[reverse_count].character = (unsigned char)i;
            reverse_count++;
        }
    }

    long data_start_pos = ftell(input);
    long padding_pos = -1;
    int padding = 0;
    
    while (fgets(line, sizeof(line), input)) {
        if (strncmp(line, "PADDING:", 8) == 0) {
            padding_pos = ftell(input) - strlen(line);
            sscanf(line, "PADDING: %d", &padding);
            break;
        }
    }
    
    if (padding_pos == -1) {
        printf("Błąd: Nie znaleziono linii PADDING w pliku!\n");
        fclose(input);
        fclose(output);
        return 0;
    }
    fseek(input, data_start_pos, SEEK_SET);

    char current_code[MAX_CODE_LEN] = {0};
    int code_pos = 0;

    long bytes_to_read = padding_pos - data_start_pos - 1; 
    
    int byte;
    long bytes_read = 0;
    while (bytes_read < bytes_to_read && (byte = fgetc(input)) != EOF) {
        int bits_to_process = (bytes_read == bytes_to_read - 1 && padding > 0) ? (8 - padding) : 8;
        
        for (int i = 7; i >= (8 - bits_to_process); i--) {
            int bit = (byte >> i) & 1;
            current_code[code_pos++] = bit + '0';
            current_code[code_pos] = '\0';

            for (int j = 0; j < reverse_count; j++) {
                if (strcmp(current_code, reverse_codes[j].code) == 0) {
                    fputc(reverse_codes[j].character, output);
                    code_pos = 0;
                    current_code[0] = '\0';
                    break;
                }
            }
        }
        bytes_read++;
    }

    fclose(input);
    fclose(output);

    printf("Dekompresja zakończona pomyślnie!\n");
    return 1;
}

