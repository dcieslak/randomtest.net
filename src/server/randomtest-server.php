<?php

$DATABASE_FILE = "var/randomtest.db";
$db = dba_open($DATABASE_FILE, "c");

if (isset($_POST['stacktrace'])) {
    header('Content-type: text/plain');

    echo "OK\n";
    $stacktrace = $_POST['stacktrace'];
    echo $stacktrace;

    if (dba_exists($stacktrace, $db)) {
        $counter = dba_fetch($stacktrace, $db) + 1;
    }
    else {
        $counter = 1;
    }
    dba_replace($stacktrace, $counter, $db);
}
else {
    header('Content-type: text/plain');
    echo "RandomTest.net report\n\n";
    for($stacktrace = dba_firstkey($db); $stacktrace != false; $stacktrace = dba_nextkey($db)) {
        $counter = dba_fetch($stacktrace, $db);
        $arr[$stacktrace] = $counter;
    }
    // sort array by counter, keys=stacktraces left
    arsort($arr);
    foreach ($arr as $stacktrace => $counter) {
        echo "event counter: $counter\n$stacktrace\n";
    }
}

dba_close($db);

?>
