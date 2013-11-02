// RandomTest.net - probe for JavaScritp in browser

// 1. Setup the correct URL here
// RANDOMTEST_URL = "%RANDOMTEST_URL%"

Error = function (msg, fileName, lineNumber) {
    console.log("Error created: " + msg)
}

function callHttpPost(theUrl, postParam, postValue)
{
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.open( "POST", theUrl, false );
    xmlHttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    xmlHttp.setRequestHeader("Connection", "close");
    xmlHttp.send( postParam + "=" + encodeURI(postValue) );
    return xmlHttp.responseText;
}

function randomtest_error_handler (e, file, line, column, errorObj) {

    message = e
    if (typeof(e.stack) == 'undefined') {
        // Workaround if no stack is avialable
        try { null.null }
        catch (exc) { e = exc; console.log(exc.stack); }
    }
    var stack = e.stack.replace(/^[^\(]+?[\n$]/gm, '')
    .replace(/^\s+at\s+/gm, '')
    .replace(/^Object.<anonymous>\s*\(/gm, '{anonymous}()@');

    out = callHttpPost(RANDOMTEST_URL, "stacktrace", "[" + message + " at " + file + ":" + line + "]\n" + stack + "\n")
    // out = callHttpPost(RANDOMTEST_URL, "stacktrace", "[" + message + "]\n" + stack + "\n")

    return true
}

window.onerror = randomtest_error_handler

