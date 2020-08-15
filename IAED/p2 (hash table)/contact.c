/* File: contact.c
 * Description: Impelements contructor, desctructor, modifiers and selectors
 * for the contact structure which are useful for handling most commands.
 */

#include "contact.h"
#include "OOP.h"

/* The following copyright notice applies only to the strdup function which is
 part of glibc.

   Copyright (C) 1991-2019 Free Software Foundation, Inc.
   This function is part of the GNU C Library.
   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.
   */
char *strdup(const char *s) {
  size_t len = strlen(s) + 1;
  void *new = malloc(len);
  if (new == NULL)
    return NULL;
  return (char *)memcpy(new, s, len);
}

contact *create_contact(char *name, char *user, char *domain, char *phone) {
  contact *c = malloc(sizeof(contact));

  c->name = strdup(name); /* allocates only the necessary memory */
  c->user = strdup(user);
  c->domain = strdup(domain);
  c->phone = strdup(phone);

  return c;
}

void free_contact(void *v) {
  contact *c = v;
  free(c->name); /* to avoid memory leaks */
  free(c->user);
  free(c->domain);
  free(c->phone);
  free(c);
}

char *get_contact_name(void *v) { return ((contact *)v)->name; }

void print_contact(void *v) {
  contact *c = v;
  printf("%s %s@%s %s\n", c->name, c->user, c->domain, c->phone);
}

void change_email(contact *c, char *new_user, char *new_domain) {
  free(c->user); /* We don't need the old email any more */
  free(c->domain);

  c->user = strdup(new_user);
  c->domain = strdup(new_domain);
}
