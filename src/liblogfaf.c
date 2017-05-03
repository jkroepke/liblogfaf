/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <pthread.h>

#if defined(__APPLE__)
#include <crt_externs.h>
#define HOST_NAME_MAX 255
#else
#define HOST_NAME_MAX _POSIX_HOST_NAME_MAX
#endif

#define MAX_MESSAGE_LEN 65536

// From RFC3164
static const char *months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

typedef struct {
    char hostname[HOST_NAME_MAX];
    char progname[1024];

    int syslog_facility;
    const char *syslog_tag;

    struct addrinfo *serveraddr;
    struct addrinfo *bind_ip;
    int sockfd;

    pthread_mutex_t lock;
} SharedData;

static SharedData shared_data = { "", "" };

#ifdef NDEBUG
#define DBG(x)
#else
#define DBG(x) debugprintf x

static void debugprintf(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fflush(stderr);
}
#endif

static void set_defaults(SharedData *sd) {
    char *slash_ptr = strrchr(sd->progname, '/');
    // If progname contains a slash, extract basename to use it as syslog tag
    sd->syslog_tag = slash_ptr ? slash_ptr + 1 : sd->progname;
    sd->syslog_facility = 0;
}

static void logmessage(SharedData *sd, int priority, const char *message) {
    DBG(("liblogfaf: logmessage(%d, %s)\n", priority, message));
    printf("%s", message);
}

__attribute__((constructor)) static void _liblogfaf_init(void) {
    DBG(("liblogfaf: init()\n"));
}

__attribute__((destructor)) static void _liblogfaf_fini(void) {
    DBG(("liblogfaf: fini()\n"));
    if (pthread_mutex_destroy(&shared_data.lock) != 0) {
        fprintf(stderr, "liblogfaf: pthread_mutex_destroy() failed\n");
        exit(1);
    }
}

void openlog(const char *ident, int option, int facility) {
    DBG(("liblogfaf: openlog(%s, %d, %d)\n", ident, option, facility));
    // making use of the `option` parameter can be added here if you need it
}

void closelog(void) {
    DBG(("liblogfaf: closelog()\n"));
}

void __syslog_chk(int priority, int flag, const char *format, ...) {
    DBG(("liblogfaf: __syslog_chk(%d, %d, %s)\n",
         priority, flag, format));
    va_list ap;
    char str[MAX_MESSAGE_LEN];
    va_start(ap, format);
    vsnprintf(str, MAX_MESSAGE_LEN, format, ap);
    va_end(ap);
    logmessage(&shared_data, priority, str);
}

void syslog(int priority, const char *format, ...) {
    DBG(("liblogfaf: syslog(%d, %s)\n", priority, format));
    va_list ap;
    char str[MAX_MESSAGE_LEN];
    va_start(ap, format);
    vsnprintf(str, MAX_MESSAGE_LEN, format, ap);
    va_end(ap);
    logmessage(&shared_data, priority, str);
}

