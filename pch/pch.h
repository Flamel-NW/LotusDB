#ifndef PCH_H
#define PCH_H

#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdbool.h>

#define FILENAME_MAX_LEN 64 

#define KEY_MAX_LEN 32
#define VALUE_MAX_LEN 256

#define WAL_PATH "./log/wal"

#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))

#define STDERR(format, ...) fprintf(stderr, format, ##__VA_ARGS__ ); putchar('\n');
#define STDERR_FUNC_LINE() \
    STDERR("file: %s, func: %s, line: %d", __FILE__, __func__, __LINE__);

long getTimestamp();

#endif
