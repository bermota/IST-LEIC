/* File: OOP.h
 * Description: Defines two types of functions useful for implementing
 * data abstraction.
 */

#ifndef OOP_H
#define OOP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Since we ought to avoid code repetition and aim to achieve some
 * abstraction, we must implement the following types of functions */

/* destructors free all the memory of a given pointer */
typedef void (*destructor)(void *);

/* returns a string related to an object wich is always a string */
typedef char *(*key_getter)(void *);

#endif
