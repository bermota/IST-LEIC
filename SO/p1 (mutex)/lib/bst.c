#include "bst.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void insertDelay(int cycles)
{
    for (int i = 0; i < cycles; i++) {
    }
}

node* new_node(char* key, int inumber)
{
    node* p = malloc(sizeof(node));
    if (!p) {
        perror("new_node: no memory for a new node");
        exit(EXIT_FAILURE);
    }
    size_t size = strlen(key) + 1;
    p->key = malloc(sizeof(char) * size);

    strncpy(p->key, key, size);
    p->inumber = inumber;
    p->left = NULL;
    p->right = NULL;
    return p;
}

int max(int a, int b)
{
    return a > b ? a : b;
}

/* returns NULL if it has not a node with given key */
node* search(node* p, char* key)
{
    insertDelay(DELAY);
    if (!p)
        return NULL;

    int comp = strcmp(key, p->key);
    if (comp < 0)
        return search(p->left, key);
    else if (comp > 0)
        return search(p->right, key);
    else
        return p;
}

/* inicializes a new node and inserts it on the bst.
 * returns a pointer to new node. */
node* insert(node* p, char* key, int inumber)
{
    insertDelay(DELAY);
    if (!p) {
        return new_node(key, inumber);
    }

    int comp = strcmp(key, p->key);
    if (comp < 0) {
        p->left = insert(p->left, key, inumber);
    } else if (comp > 0) {
        p->right = insert(p->right, key, inumber);
    } else
        p->inumber = inumber;
    return p;
}

node* find_min(node* p)
{
    if (p->left != NULL)
        return find_min(p->left);
    else
        return p;
}

node* remove_min(node* p)
{
    if (p->left == NULL)
        return p->right;

    p->left = remove_min(p->left);
    return p;
}

/* alters bst and frees memory */
node* remove_item(node* p, char* key)
{
    insertDelay(DELAY);
    if (!p)
        return NULL;

    int comp = strcmp(key, p->key);
    if (comp < 0)
        p->left = remove_item(p->left, key);
    else if (comp > 0)
        p->right = remove_item(p->right, key);
    else {
        node* l = p->left;
        node* r = p->right;
        node* m;
        free(p->key);
        free(p);

        if (r == NULL)
            return l;

        m = find_min(r);
        m->right = remove_min(r);
        m->left = l;

        return m;
    }

    return p;
}

/* also frees all nodes of trees (including strings) */
void free_tree(node* p)
{
    if (!p)
        return;

    free_tree(p->left);
    free_tree(p->right);
    free(p->key);
    free(p);
}

/* graphical (in text) representation of BST */
void print_tree_2(FILE* fp, node* p, int l)
{
    if (p) {
        print_tree_2(fp, p->left, l + 1);
        fprintf(fp, "%*s%s\n", 2 * (l + 1), "", p->key); /* 2*(l+1) spaces */
        print_tree_2(fp, p->right, l + 1);
    }
}

/* Adds newline to beginning of file and prints the BST to given file */
void print_tree(FILE* fp, node* p)
{
    fprintf(fp, "\n");
    print_tree_2(fp, p, 0);
}
