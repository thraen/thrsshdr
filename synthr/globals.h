#pragma once

#define DBG 1
#define NFO 1
#define ERR 1

#include <stdio.h>
#define err(...) do { if (ERR) fprintf(stderr, __VA_ARGS__);                 } while (0)
#define dbg(...) do { if (DBG)  printf(        __VA_ARGS__); fflush(stdout); } while (0)
#define nfo(...) do { if (NFO)  printf(        __VA_ARGS__);                 } while (0)

#define endl {printf("\n")}

#include <stdlib.h>
#define err_exit(...)    { fprintf(stderr, __VA_ARGS__); exit(1); }
#define err_fuckoff(...) { fprintf(stderr, __VA_ARGS__); fuckoff(); }
#define perr_exit(msg)   { perror(msg); exit(1); }

#define fuckoff_if(...) if (__VA_ARGS__)  err_fuckoff("OH DEAR: " #__VA_ARGS__)
#define exit_if(...)    if (__VA_ARGS__)  err_exit   ("OH DEAR: " #__VA_ARGS__)

#define exit_ifnot(...)    if (!__VA_ARGS__) err_exit ("OH DEAR: " !#__VA_ARGS__)
#define fuckoff_ifnot(...) if (!__VA_ARGS__) err_exit ("OH DEAR: " !#__VA_ARGS__)

#define pexit_if(...)    if (__VA_ARGS__)  perr_exit("OH DEAR: " #__VA_ARGS__)
#define pexit_ifnot(...) if (!__VA_ARGS__) perr_exit("OH DEAR: " !#__VA_ARGS__)

#define arrlen(arr) (sizeof(arr) / sizeof(arr[0]))
// #define membsize(type, member) sizeof(((type *)0)->member)

#ifdef DEBUG
    #include <assert.h>
    #define _assert(...) assert(__VA_ARGS__)
#else
    #define _assert(...) 
#endif

#define twopi     6.283185307179586
#define _2pi      6.283185307179586
#ifndef pi
#define pi        3.141592653589793
#endif
#define halfpi    1.5707963267948966
#define quarterpi 0.7853981633974483

// #define min(a,b) (((a)<(b))?(a):(b))
// #define max(a,b) (((a)>(b))?(a):(b))
