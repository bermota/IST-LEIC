/* File: hash_table.c
 * Description: Implementation of basic operations for ht ADT.
 * a domain.
 */

#include "hash_table.h"

node *create_node(void *data) {
  node *n = malloc(sizeof(node));
  n->prev = NULL;
  n->next = NULL;
  n->data = data;

  return n;
}

void free_node(ht *h, node *n) {
  h->free_data(n->data);
  free(n);
}

ht *create_ht(int size, destructor destroy_data, key_getter get_key_data) {
  int i;

  ht *h = malloc(sizeof(ht));
  h->size = size;
  h->free_data = destroy_data;
  h->get_data_key = get_key_data;
  h->dll_head = NULL; /* inicialize the dll with no elements */
  h->dll_tail = NULL;
  h->row_heads = malloc(sizeof(node *) * size);

  for (i = 0; i < size; ++i)
    h->row_heads[i] = NULL; /* every row has no elements in the beginning */

  return h;
}

void free_ht(ht *h) {
  node *current, *next;
  /* frees all the elements of the dll */
  for (current = h->dll_head; current; current = next) {
    next = current->next;
    free_node(h, current);
  }

  free(h->row_heads);
  free(h);
}

/* This function's implementation was copied from the
 * class's slides. I used the register keyword to force a compiler
 * optimization which is making it slightly faster, */
int hashU(register char *v, register int M) {
  register int h, a = 31415;
  register const int b = 27183;
  for (h = 0; *v != '\0'; v++, a = a * b % (M - 1))
    h = (a * h + *v) % M;
  return h;
}

void *find_in_ht(ht *h, char *key) {
  node *n = h->row_heads[hashU(key, h->size)];
  for (; n; n = n->ht_next)
    if (!strcmp(h->get_data_key(n->data), key))
      return n->data; /* we have found a node with the same key */

  return NULL; /* we haven't found any to match */
}

void add_to_ht(ht *h, void *data) {
  node *new_node;

  /* hash the key to get the row */
  int row = hashU(h->get_data_key(data), h->size);

  new_node = create_node(data);
  new_node->prev = h->dll_tail;

  new_node->ht_next = h->row_heads[row]; /* adding to the single linked list */
  h->row_heads[row] = new_node;          /* change the head in the sll*/

  /* adding the element to the double linked list */
  if (h->dll_tail == NULL)
    h->dll_head = new_node;
  else
    h->dll_tail->next = new_node;

  h->dll_tail = new_node;
}

void remove_in_ht(ht *h, char *key) {
  int row = hashU(key, h->size);
  node *n, *prev, *head = h->row_heads[row];

  for (prev = NULL, n = head; n; prev = n, n = n->ht_next) {
    if (!strcmp(key, h->get_data_key(n->data))) { /* if same key */
      if (n == head) /* if first element of the line */
        h->row_heads[row] = n->ht_next;

      else
        prev->ht_next = n->ht_next;

      if (!n->prev) /* if first element in single linked list */
        h->dll_head = n->next;

      else
        n->prev->next = n->next;

      if (!n->next) /* if last element */
        h->dll_tail = n->prev;

      else
        n->next->prev = n->prev;

      free_node(h, n); /* deallocates all the memory */
      return;          /* earlier return avoid unnecessary computation */
    }
  }
}

void apply_all_in_ht(ht *h, void (*do_something)(void *)) {
  node *n;
  for (n = h->dll_head; n; n = n->next)
    do_something(n->data);
}
