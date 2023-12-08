/* 
 * freadline.h - a utility function to safely read one line of input
 * Safely read a line of input into `buf` (with room for `len` chars).
 *
 * David Kotz, April 2016, 2017
 * CS 50, Fall 2022
 */

#ifndef __READLINE_H__
#define __READLINE_H__

#include <stdio.h>
#include <stdbool.h>

/* freadline - Safely read a line of input into `buf`.
 *
 * Caller provides buffer `buf` with room for `len` characters.
 * Caller provides open file `fp`.
 * We fill `buf` by reading characters from file `fp`,
 * guaranteeing we won't over-run the end of the buffer,
 * guaranteeing a newline-terminated, null-terminated string in the buffer;
 * returning true if success and false if EOF or any error.
 * If the user enters more than len-2 characters, that is considered an error.
 * If end-of-file is reached before newline, that is considered an error.
 * If an error occurs, buf may have been written but should be untrusted.
 */
extern bool freadline(FILE *fp, char *buf, const int len);

#endif //  __READLINE_H__