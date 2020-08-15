/* bst.h */
#ifndef BST_H
#define BST_H
#include <stdio.h>

/* Measured in CPU cycles. Made to emulate CPU cycles. */
#define DELAY 5000

/* Node of a BST represents a file.
 * Has a key (file name) and a unique inumber. */
typedef struct node {
    char* key; /* used for representing file */
    int inumber; /* unique number for each node */

    struct node* left;
    struct node* right;
} node;

void insertDelay(int cycles);
node* search(node* p, char* key);
node* insert(node* p, char* key, int inumber);
node* find_min(node* p);
node* remove_min(node* p);
node* remove_item(node* p, char* key);
void free_tree(node* p);
void print_tree(FILE* fp, node* p);

#endif /* BST_H */
