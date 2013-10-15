<?php

/*
Copyright: randomtest.net
*/

function format_backtrace($frameSkip = 2) {
    $trace = '';
    foreach (debug_backtrace() as $frameOffset => $frameInfo) {
        if ($frameOffset < $frameSkip) {
            continue;
        }

        $file = basename($frameInfo['file']);
        $line = $frameInfo['line'];
        $function = $frameInfo['function'];

        $trace .= "$file:$line $function()\n";
    }

    return $trace;
}

function send_stacktrace ($comment) {
    $RANDOMTEST_URL = getenv("RANDOMTEST_URL");

    if (!$RANDOMTEST_URL) {
        trigger_error("missing RANDOMTEST_URL in environment, skipping report");
        return true;
    }

    $handle = curl_init();

    $stacktrace = format_backtrace(2);
    $stacktraceWithDecor = "[$comment]\n${stacktrace}";
    $encodedStacktrace = urlencode($stacktraceWithDecor);

    curl_setopt($handle, CURLOPT_URL, $RANDOMTEST_URL);
    curl_setopt($handle, CURLOPT_POST, 1);
    curl_setopt($handle, CURLOPT_POSTFIELDS, "stacktrace=$encodedStacktrace");
    curl_exec($handle);
    curl_close($handle);

    return true;
}

function randomtest_error_handler ($num, $str, $file, $line, $context = null) {
    send_stacktrace("$str");
}

function randomtest_exception_handler (Exception $e) {
    send_stacktrace("$e");
}

error_reporting(E_ALL);
set_error_handler("randomtest_error_handler", E_ALL | E_STRICT);
set_exception_handler("randomtest_exception_handler");

?>
