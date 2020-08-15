/* File: hash_table.h
 * Description: Defines the ht data structure which has O(1) lookup time.
 */

#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "OOP.h"
#include "contact.h"

/* These are elements of the ht type described below */
typedef struct stru_node {
  void *data;
  struct stru_node *prev, *next, *ht_next;
} node;

/* ht is a hybrid ADT which inherits properties from linked lists and
 * hash tables. Below are time complexities for basic operations

 Operation  |   Time complexity
 Insertion  |   O(1)
 Removal    |   O(1)
 Access     |   O(1)

 The user of this ADT must supply a pointer to a destructor which
 frees all the memory of the inserted element. He/she must also
 supply a pointer to a function which returns a key (pointer to char)
 of the data type.

 There musn't be elements with the same key on ht. It's the user reponsability
 to verify this. If there are repeated elements, find_ht will return one
 element which has the correct key.

 Theoretically different data types could be inserted in this data structure,
 however that is not an inteded use and is not recommended.

 */
typedef struct {
  destructor free_data; /* a destructor for the type of data in the hash tale,
                           not for the node but for the content in the node*/
  key_getter get_data_key;
  int size;                  /* how many rows in the hash table */
  node **row_heads;          /* array of pointer to nodes which are heads */
  node *dll_head, *dll_tail; /* to keep the order of insertion */

} ht;

/* creates a node with inicialized pointers to NULL */
node *create_node(void *data);

/* destructor for node, deallocates all its memory, it needs to know the data
 * type of that element, and that is given by the ht struct */
void free_node(ht *, node *);

/* creates a hash table for a certain data type*/
ht *create_ht(int size, destructor destroy_data, key_getter get_key_data);

/* deallocates all the memory for a hash table. Frees all the nodes */
void free_ht(ht *);

/* returns a hash (int) between 0 and M-1 based on the string key */
int hashU(char *key, int M);

/* returns a pointer to data if it has the same key as argument.
 * returns NULL pointer if none is found. */
void *find_in_ht(ht *, char *key);

/* Adds any data type to ht. Does not check for repeated elements in ht */
void add_to_ht(ht *, void *data);

/* remove an element of a hash table by key and deallocates it's memory */
void remove_in_ht(ht *, char *key);

/* applies do_something to all elements (data atribute) of the hash table by
 * insertion order. This is somewhat similar to functional programming
 * languages' map combinator. This implementation is useful for printing
 * all the elements in the hash table */
void apply_all_in_ht(ht *, void (*do_something)(void *));

#endif
