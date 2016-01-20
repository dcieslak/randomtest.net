/**
 * randomtest.net
 */
#include <arpa/inet.h>
#include <cxxabi.h>
#include <errno.h>
#include <execinfo.h>
#include <fcntl.h>
#include <libgen.h>
#include <malloc.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <ucontext.h>
#include <unistd.h>
#include <stdlib.h>

#include "randomtest-common.h"

void install_probe(void) __attribute__((constructor));


void find_process_name(char *buf, int max_size) {
    char linkname[512]; /* /proc/<pid>/exe */
    /* Get our PID and build the name of the link in /proc */
    int pid = getpid();
    snprintf(linkname, sizeof(linkname), "/proc/%i/exe", pid);
    /* Now read the symbolic link */
    int ret = readlink(linkname, buf, max_size);
    buf[ret] = 0;
}

/** Print a demangled stack backtrace of the caller function to FILE* out. */
void print_event_to_FILE(const char *source, FILE *out) {
    char* ptr;

    /* replace all newlines with spaces to show them properly in a stacktrace */
    char buffer[BUFSIZE];
    strncpy(buffer, source, BUFSIZE);
    for (ptr = buffer; *ptr; ptr++) {
        if (*ptr == '\n') {
            *ptr = ' ';
        }
    }

    char processName[512];
    find_process_name(processName, sizeof(processName));
    char* baseName = basename(processName);

    fprintf(out, "RTN: [%s %s]\n", baseName, buffer);

    // storage array for stack trace address data
    void *addrlist[MAXFRAMES + 1];

    // retrieve current stack addresses
    int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(addrlist[0]));

    if (addrlen == 0) {
        fprintf(out, "RTN: <empty, possibly corrupt>\n");
        return;
    }

    // resolve addresses into strings containing "filename(function+address)",
    // this array must be free()-ed
    char **symbollist = backtrace_symbols(addrlist, addrlen);

    // allocate string which will be filled with the demangled function name
    size_t funcnamesize = 256;
    char *funcname = (char *) malloc(funcnamesize);

    // iterate over the returned symbol lines. skip first two,
    // (addresses of this function and handler)
    for (int i = SKIP_FRAMES; i < addrlen; i++) {
        char *begin_name = 0, *begin_offset = 0, *end_offset = 0;

        // find parentheses and +address offset surrounding the mangled name:
        // ./module(function+0x15c) [0x8048a6d]
        for (char *p = symbollist[i]; *p; ++p) {
            if (*p == '(') {
                begin_name = p;
            } else if (*p == '+') {
                begin_offset = p;
            } else if (*p == ')' && begin_offset) {
                end_offset = p;
                break;
            }
        }

        if (begin_name && begin_offset && end_offset
                && begin_name < begin_offset) {
            *begin_name++ = '\0';
            *begin_offset++ = '\0';
            *end_offset = '\0';

            // mangled name is now in [begin_name, begin_offset) and caller
            // offset in [begin_offset, end_offset). now apply
            // __cxa_demangle():

            int status;
            char *retName = abi::__cxa_demangle(begin_name,
                                                funcname, &funcnamesize, &status);
            if (status == 0) {
                funcname = retName; // use possibly realloc()-ed string
                fprintf(out, "RTN: %s: %s\n",
                        symbollist[i], funcname);

            } else {
                // demangling failed. Output function name as a C function with
                // no arguments.
                fprintf(out, "RTN: %s: %s()\n",
                        symbollist[i], begin_name);
            }

            // Skip long list of repeating clone()
            if (strstr(begin_name, "clone()")) {
                break;
            }
        } else {
            // couldn't parse the line? print the whole line.
            char* braceOffset = index(symbollist[i], (int) '[');
            if (braceOffset) {
                *braceOffset = '\0';
            }
            fprintf(out, "RTN: %s: ??\n", symbollist[i]);
        }
    }

    fprintf(out, "RTN: [end %s]\n", buffer);

    free(funcname);
    free(symbollist);
}



// sends current backtrace to default crash report collector
void record_event(const char *source) {

    char rawStacktrace[BUFSIZE];

    FILE* memoryFile = fmemopen(rawStacktrace, BUFSIZE, "wt");
    print_event_to_FILE(source, memoryFile);
    fclose(memoryFile);

    send_stacktrace_by_curl(rawStacktrace);
}

static void noop_handler(int, siginfo_t *, void *) {
}


static void abort_process() {
    struct sigaction sigact;
    sigact.sa_flags = SA_RESTART | SA_SIGINFO;
    sigact.sa_sigaction = noop_handler;
    sigaction(SIGABRT, &sigact, (struct sigaction *)NULL);
    abort();
}


static void sigsegv_handler(int, siginfo_t *, void *) {
    record_event("sigsegv_handler");
    abort_process();
}

static void sigusr1_handler(int, siginfo_t *, void *) {
    record_event("sigusr1_handler");
}

static void sigbus_handler(int, siginfo_t *, void *) {
    record_event("sigbus_handler");
    abort_process();
}

static void sigfpe_handler(int, siginfo_t *, void *) {
    record_event("sigfpe_handler");
    abort_process();
}

static void sigabort_handler(int, siginfo_t *, void *) {
    record_event("sigabort_handler");
    abort_process();
}

static void terminate_handler() {
    record_event("terminate_handler");
    abort_process();
}

static void unexpected_handler() {
    record_event("unexpected_handler");
    abort_process();
}

void install_probe() {
    struct sigaction sigact;

    sigact.sa_flags = SA_RESTART | SA_SIGINFO;

    sigact.sa_sigaction = sigsegv_handler;
    sigaction(SIGSEGV, &sigact, (struct sigaction *)NULL);

    sigact.sa_sigaction = sigbus_handler;
    sigaction(SIGBUS, &sigact, (struct sigaction *)NULL);

    sigact.sa_sigaction = sigfpe_handler;
    sigaction(SIGFPE, &sigact, (struct sigaction *)NULL);

    sigact.sa_sigaction = sigabort_handler;
    sigaction(SIGABRT, &sigact, (struct sigaction *)NULL);

    sigact.sa_sigaction = sigusr1_handler;
    sigaction(SIGUSR1, &sigact, (struct sigaction *)NULL);

    std::set_terminate(terminate_handler);
    std::set_unexpected(unexpected_handler);

}

