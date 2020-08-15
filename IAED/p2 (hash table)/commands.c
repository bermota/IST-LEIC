/* File: commands.c
 * Description: Defines functions which handle user commands.
 * Each command 'calls' directly a function which handles user input and
 * receives as arguments a pointer to the data which it will modify.
 */

#include "commands.h"
#include "domain.h"

void cmd_a(ht *contacts, ht *domains) {
  /* buffers for handling user input */
  char name[MAXNAME], user[MAXUSER], domain[MAXDOMAIN], phone[MAXPHONE];
  contact *new_contact;

  scanf("%s %[^@]@%s %s", name, user, domain, phone);

  if (find_in_ht(contacts, name)) {
    printf("Nome existente.\n");
    return;
  }

  new_contact = create_contact(name, user, domain, phone);
  add_to_ht(contacts, new_contact);
  incr_domain(domains, domain);
}

void cmd_l(ht *contacts) { apply_all_in_ht(contacts, print_contact); }

void cmd_p(ht *contacts) {
  char name[MAXNAME];
  contact *c;
  scanf("%s", name);
  if ((c = find_in_ht(contacts, name)))
    print_contact(c);

  else
    printf("Nome inexistente.\n");
}

void cmd_r(ht *contacts, ht *domains) {
  char name[MAXNAME];
  contact *c;
  scanf("%s", name);
  if ((c = find_in_ht(contacts, name))) {
    decr_domain(domains, c->domain);
    remove_in_ht(contacts, name);
  } else
    printf("Nome inexistente.\n");
}

void cmd_e(ht *contacts, ht *domains) {
  char name[MAXNAME], user[MAXUSER], domain[MAXDOMAIN];
  contact *c;

  scanf("%s %[^@]@%s", name, user, domain);

  if ((c = find_in_ht(contacts, name))) {
    /* if the domain is different -**/
    if (strcmp(c->domain, domain)) {
      decr_domain(domains, c->domain);
      incr_domain(domains, domain);
    }
    change_email(c, user, domain);
  }

  else
    printf("Nome inexistente.\n");
}

void cmd_c(ht *domains) {
  char domain[MAXDOMAIN];
  scanf("%s", domain);
  print_domain_freq(domains, domain);
}
