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
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <sem182.h>
#include <sys/ipc.h>
#include <sys/shm.h>

/*
 * ------------------------------------------------------------- functions --
 */
extern size_t get_ringbuffer_size(int argc, char* argv[]);
extern int get_semid(int initval);
extern int* get_shm(size_t ringbuffer_size, int flags);
extern void remove_all(void);

#endif

/*
 * =================================================================== eof ==
 */
