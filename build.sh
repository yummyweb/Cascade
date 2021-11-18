#!/bin/sh
cc -Wall vm.c debug.c memory.c value.c chunk.c -o cascade main.c
