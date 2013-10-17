/**
 * randomtest.net
 */
#include <assert.h>
#include <libgen.h>
#include <ctype.h>
#include <curl/curl.h>
#include <arpa/inet.h>
#include <assert.h>
#include <cxxabi.h>
#include <errno.h>
#include <execinfo.h>
#include <fcntl.h>
#include <malloc.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <ucontext.h>
#include <unistd.h>
#include <netdb.h>

#define BUFSIZE 1024
#define MAXFRAMES 16
#define SKIP_FRAMES 4

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
void send_event(const char *source, FILE *out) {
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

    fprintf(out, "[%s %s]\n", baseName, buffer);

    // storage array for stack trace address data
    void *addrlist[MAXFRAMES + 1];

    // retrieve current stack addresses
    int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(addrlist[0]));

    if (addrlen == 0) {
        fprintf(out, "  <empty, possibly corrupt>\n");
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
                fprintf(out, "%s: %s\n",
                        symbollist[i], funcname);

            } else {
                // demangling failed. Output function name as a C function with
                // no arguments.
                fprintf(out, "%s: %s()\n",
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
            fprintf(out, "%s: ??\n", symbollist[i]);
        }
    }

    free(funcname);
    free(symbollist);
}



/* BEGIN Geek Hideout -- http://www.geekhideout.com/urlcode.shtml -- public domain */
/* Converts a hex character to its integer value */
char from_hex(char ch) {
    return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

/* Converts an integer value to its hex character*/
char to_hex(char code) {
    static char hex[] = "0123456789abcdef";
    return hex[code & 15];
}

/* Returns pointer to last byte with '\0' */
char *url_encode(const char* prefix, const char *str, char* out, const char* end) {
    const char *pstr;
    char *pbuf = out;

    assert(prefix);
    assert(str);
    assert(out);
    assert(end > out);

    pstr = prefix;
    while (*pstr && pbuf < end - 1) {
        *pbuf++ = *pstr++;
    }
    
    pstr = str;
    while (*pstr && pbuf < end - 1) {
        if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~') 
            *pbuf++ = *pstr;
        else if (*pstr == ' ') 
            *pbuf++ = '+';
        else 
            *pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex(*pstr & 15);
        pstr++;
    }
    *pbuf = '\0';
    return pbuf;
}
/* END Geek Hideout -- http://www.geekhideout.com/urlcode.shtml -- public domain */



// sends current backtrace to default crash report collector
void record_event(const char *source) {

    const char *url = getenv("RANDOMTEST_URL");
    char rawStacktrace[BUFSIZE];
    char encodedStacktrace[BUFSIZE];

    FILE* file = fmemopen(rawStacktrace, BUFSIZE, "wt");
    send_event(source, file);
    fclose(file);
    fprintf(stderr, "%s", rawStacktrace);

    if (!url) {
        fprintf(stderr, "WARN: RANDOMTEST_URL not set, crash reports skipped\n");
        return;
    }

    url_encode("stacktrace=", rawStacktrace, encodedStacktrace, encodedStacktrace + BUFSIZE);

    CURL* handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_POST, 1);
    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, encodedStacktrace);
    CURLcode code = curl_easy_perform(handle);
    if (code != 0) {
        fprintf(stderr, "ERROR: %s call failed with error %d\n", url, code);
        return;
    }
    curl_easy_cleanup(handle);

}

static void noop_handler(int, siginfo_t *, void *) {
}


static void abort_process() {
    struct sigaction sigact;
    sigact.sa_flags = SA_RESTART | SA_SIGINFO;
    sigact.sa_sigaction = noop_handler;
    sigaction(SIGABRT, &sigact, (struct sigaction *)NULL);
    exit(1);
}


static void sigsegv_handler(int, siginfo_t *, void *) {
    record_event("sigsegv_handler");
    abort_process();
}

static void sigbus_handler(int, siginfo_t *, void *) {
    record_event("sigbus_handler");
    abort_process();
}

void install_probe() {
    struct sigaction sigact;

    sigact.sa_flags = SA_RESTART | SA_SIGINFO;

    sigact.sa_sigaction = sigsegv_handler;
    sigaction(SIGSEGV, &sigact, (struct sigaction *)NULL);

    sigact.sa_sigaction = sigbus_handler;
    sigaction(SIGBUS, &sigact, (struct sigaction *)NULL);

}

