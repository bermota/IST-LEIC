/* File: contact.h
 * Description: Contact is a structure which contains basic information about
 * a contact. Memory is dynamically allocated for efficiency reasons.
 */

#ifndef CONTACT_H
#define CONTACT_H

#include "OOP.h"

/* Internal representation for a contact of: name user@domain phone */
typedef struct {
  char *name, *user, *domain, *phone;
} contact;

/* glibc's implementation of strdup which we are required to copy due to
 * restrictions applied by compiler flags */
char *strdup(const char *s);

/* contact constructor */
contact *create_contact(char *name, char *user, char *domain, char *phone);

/* contact destructor */
void free_contact(void *contact);

/* returns the KEY (i.e. name )of a contact */
char *get_contact_name(void *contact);

/* shows output to user using printf */
void print_contact(void *contact);

/* modifier for conatct */
void change_email(contact *c, char *new_user, char *new_domain);

#endif
