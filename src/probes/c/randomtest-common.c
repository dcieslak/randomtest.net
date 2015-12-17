#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <curl/curl.h>

#include "randomtest-common.h"

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

void send_stacktrace_by_curl(const char* rawStacktrace) {

    const char *fileNameToSave = getenv("RANDOMTEST_FILE");
    const char *url = getenv("RANDOMTEST_URL");
    const char *version = getenv("RANDOMTEST_VERSION");

    char encodedStacktrace[BUFSIZE];

    if (fileNameToSave) {
        FILE* f = fopen(fileNameToSave, "at");
        fprintf(f, "%s", rawStacktrace);
        fclose(f);
    }
    else {
        fprintf(stderr, "%s", rawStacktrace);
    }

    /* no URL or empty value -> no reporting */
    if (!url || url[0] == '\0') {
        // fprintf(stderr, "WARN: RANDOMTEST_URL not set, crash reports skipped\n");
        return;
    }

    char* buf_ptr = url_encode("stacktrace=", rawStacktrace, encodedStacktrace, encodedStacktrace + BUFSIZE);
    *buf_ptr++ = '&';
    if (version) {
        buf_ptr = url_encode("version=", version, buf_ptr, encodedStacktrace + BUFSIZE);
    }

    CURL* handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_POST, 1);
    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, encodedStacktrace);

    /* Fix HTTP 417 error code */
    struct curl_slist *list = NULL;
    list = curl_slist_append(list, "Expect:");
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, list);

    CURLcode code = curl_easy_perform(handle);
    if (code != 0) {
        fprintf(stderr, "RTN: ERROR: %s call failed with error %d\n", url, code);
        return;
    }
    curl_easy_cleanup(handle);

}


