/**
 * @file myipc.h
 * Betriebssysteme myipc Header File.
 * Beispiel 3
 *
 * @author Aleksandar Pavlovic, Johanna Hoffmann, Michael Käfer
 * @date 2017/06/03
 *
 * @version 1
 *
 * @todo Review
 *
 */

/*
 * --------------------------------------------------------------- defines --
 */
#ifndef MYIPCLIB_H
#define MYIPCLIB_H

/*
 * -------------------------------------------------------------- includes --
 */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>

/*
 * ------------------------------------------------------------- functions --
 */
extern int get_ringbuffer_size(int argc, char* argv[]);

#endif

/*
 * =================================================================== eof ==
 */
