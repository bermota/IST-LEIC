/* File: main.c
 * Description: Contains loop for handling user commands and inicialization
 * of data structures for storing user information. */

#include "commands.h"
#include "hash_table.h"

int main() {
  char cmd; /* stores the command letter given by the user */

  /* it's a kind of hash table which can store elements of type contact */
  ht *contacts = create_contact_ht(HT_CONTACTS_SIZE);
  ht *domains = create_domain_ht(HT_DOMAINS_SIZE);
  while (1) {
    scanf("%c", &cmd);
    switch (cmd) {
    case 'a': /* adding a contact */
      cmd_a(contacts, domains);
      break;
    case 'l': /* listing all contacts by order */
      cmd_l(contacts);
      break;
    case 'p': /* pretty printing a contact */
      cmd_p(contacts);
      break;
    case 'r': /* removing a contact by name */
      cmd_r(contacts, domains);
      break;
    case 'e': /* editing the email of a contact */
      cmd_e(contacts, domains);
      break;
    case 'c': /* counting how many times a domain occurs in all contacts */
      cmd_c(domains);
      break;
    case 'x': /* deallocates all the memory */
      free_ht(contacts);
      free_ht(domains);
      return EXIT_SUCCESS;
    }
  }

  return EXIT_SUCCESS;
}
