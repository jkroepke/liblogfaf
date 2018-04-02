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
#include <sys/stat.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <pthread.h>

#define MAX_MESSAGE_LEN 65536

typedef struct {
    char progname[1024];
    const char *syslog_tag;
} SharedData;

static SharedData shared_data = {  };

const char * target;

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
}

static void init_progname(SharedData *sd) {
#if defined(__APPLE__)
    sscanf(*_NSGetProgname(), "%1023s", sd->progname);
#elif defined(__FreeBSD__)
    sscanf(getprogname(), "%1023s", sd->progname);
#else
    FILE* cmdline = fopen("/proc/self/cmdline", "rb");
    if (cmdline) {
        fscanf(cmdline, "%1023s", sd->progname);
        fclose(cmdline);
    }
#endif
}

__attribute__((constructor)) static void _liblogfaf_init(void) {
    DBG(("liblogfaf: init()\n"));
    init_progname(&shared_data);
    set_defaults(&shared_data);
    target = getenv("LIBLOGFAF_SENDTO");
    DBG(("liblogfaf: LIBLOGFAF_SENDTO: %s\n", getenv("LIBLOGFAF_SENDTO")));
}

__attribute__((destructor)) static void _liblogfaf_fini(void) {
    DBG(("liblogfaf: fini()\n"));
}

void openlog(const char *ident, int option, int facility) {
    DBG(("liblogfaf: openlog(%s, %d, %d)\n", ident, option, facility));
    if (ident)
        shared_data.syslog_tag = ident;
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
    
    DBG(("liblogfaf: pipe messages to %s\n", target));
        
    if(NULL != target) { 
        DBG(("liblogfaf: fopen(%s)\n", target));
        FILE* fd = fopen(target, "wb");
        fprintf(fd, "%s: %s\n", shared_data.syslog_tag, str);
        fclose(fd);
    } else {
        fprintf(stdout, "%s: %s\n", shared_data.syslog_tag, str);
        fflush(stdout);
    }
}

void syslog(int priority, const char *format, ...) {
    DBG(("liblogfaf: syslog(%d, %s)\n", priority, format));
    va_list ap;
    char str[MAX_MESSAGE_LEN];
    va_start(ap, format);
    vsnprintf(str, MAX_MESSAGE_LEN, format, ap);
    va_end(ap);
    
    DBG(("liblogfaf: pipe messages to %s\n", target));
        
    if(NULL != target) { 
        DBG(("liblogfaf: fopen(%s)\n", target));
        FILE* fd = fopen(target, "wb");
        fprintf(fd, "%s: %s\n", shared_data.syslog_tag, str);
        fclose(fd);
    } else {
        fprintf(stdout, "%s: %s\n", shared_data.syslog_tag, str);
        fflush(stdout);
    }
}

