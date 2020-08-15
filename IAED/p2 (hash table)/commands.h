/* File: commands.h
 * Description: Prototypes for user command handling functions.
 * Also includes buffer and hash table sizes as macros.
 */

#ifndef COMMANDS_H
#define COMMANDS_H

#include "OOP.h"
#include "contact.h"
#include "domain.h"
#include "hash_table.h"

/* It's a prime number
 * Number of rows in the hash table for contacts.
 * We may store more domains than this number, but there will be collisions */
#define HT_CONTACTS_SIZE 8191

/* Number of rows in the hash table for domains */
#define HT_DOMAINS_SIZE 1021

/* maximum size for buffers used for reading input*/
#define MAXNAME 1024
#define MAXUSER 512
#define MAXDOMAIN 512
#define MAXPHONE 64

/* contact hash tables are instances of the hash table 'class' */
#define create_contact_ht(SIZE) create_ht(SIZE, free_contact, get_contact_name)
#define create_domain_ht(SIZE) create_ht(SIZE, free_domain, get_domain_name)

/* handles UX for command a, to add an event verifying */
void cmd_a(ht *contacts, ht *domains);

/* lists all the contacts */
void cmd_l(ht *contacts);

/* prints a given contact */
void cmd_p(ht *contacts);

/* removes a given contact */
void cmd_r(ht *contacts, ht *domains);

/* edits the email of a given contact */
void cmd_e(ht *contacts, ht *domains);

/* counts the number of occurences of a given domain in the email */
void cmd_c(ht *domains);

#endif
