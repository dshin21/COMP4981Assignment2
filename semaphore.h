#pragma once

#include <stdio.h>
#include <sys/types.h>  // includes definitions for types
#include <sys/ipc.h>    // inter-process communication access structure
#include <sys/sem.h>    // semaphore facility

int create_semaphore(key_t key);

int remove_semaphore(int sid);

void P(int sid);

void V(int sid);