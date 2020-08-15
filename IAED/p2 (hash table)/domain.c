/* File: domain.c
 * Description: Implementation of basic operations for domain ADT.
 */

#include "domain.h"
#include "OOP.h"
#include "hash_table.h"

domain *create_domain(char *domain_name) {
  domain *d = malloc(sizeof(domain));
  d->domain = strdup(domain_name);
  d->frequency = 1; /* we only add domains that occur */

  return d;
}

char *get_domain_name(void *v) {
  domain *d = v;
  return d->domain;
}

void free_domain(void *v) {
  domain *d = v;
  free(d->domain);
  free(d);
}

void print_domain(domain *d) { printf("%s:%d\n", d->domain, d->frequency); }

void incr_domain(ht *h, char *domain_name) {
  domain *d;
  if ((d = find_in_ht(h, domain_name)))
    d->frequency++;

  else { /* if the domains does not exist */
    d = create_domain(domain_name);
    add_to_ht(h, d);
  }
}

void decr_domain(ht *h, char *domain_name) {
  domain *d;
  d = find_in_ht(h, domain_name);

  d->frequency--;
}

void print_domain_freq(ht *domains, char *domain_name) {
  domain *d;
  d = find_in_ht(domains, domain_name);
  if (d)
    print_domain(d);
  else /* domains that do not exist occur 0 times */
    printf("%s:0\n", domain_name);
}
