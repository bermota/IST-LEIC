/* File: domain.h
 * Description: Domain ADT is used for storing the number of occurrences of
 * a domain. It's useful for command c.
 */

#ifndef DOMAIN_H
#define DOMAIN_H

#include "OOP.h"
#include "hash_table.h"

/* Domains are the last part of the email. This type hash a key therefore
 * it may be inserted in hash tables */
typedef struct {
  char *domain;
  int frequency;
} domain;

/* inicializes a domain with 1 occurrency */
domain *create_domain(char *domain_name);

/* key getter for domain ADT */
char *get_domain_name(void *domain);

/* deallocates all the memory of a domain */
void free_domain(void *domain);

/* prints domain:<number-of-occurencies> */
void print_domain(domain *);

/* inscreases the frequency of a domain by one in a hash table */
void incr_domain(ht *domains, char *domain_name);

/* decreases the frequency of a domain by one in a hash table */
void decr_domain(ht *domains, char *domain_name);

/* prints the number of times a domains name occurs in a hash table of domains.
It also works if the domain is not in the hash table */
void print_domain_freq(ht *domains, char *domain_name);

#endif
