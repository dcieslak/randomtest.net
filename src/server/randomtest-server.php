<?php

$db_counters = dba_open("var/counters.db", "c");
$db_versions = dba_open("var/versions.db", "c");

if (isset($_POST['stacktrace'])) {
    header('Content-type: text/plain');

    echo "OK\n";
    $stacktrace = $_POST['stacktrace'];
    echo $stacktrace;

    $version = $_POST['version'];
    echo $version;

    if (dba_exists($stacktrace, $db_counters)) {
        $counter = dba_fetch($stacktrace, $db_counters) + 1;
        $versions = dba_fetch($stacktrace, $db_versions);
        if (strstr($versions, $version) === false) {
            $versions = $versions . " " . $version;
        }
    }
    else {
        $counter = 1;
        $versions = $version;
    }
    dba_replace($stacktrace, $counter, $db_counters);
    dba_replace($stacktrace, $versions, $db_versions);
}
else if (isset($_GET['reset'])) {
    // Clears local databases
    header('Content-type: text/plain');

    if ($_GET['reset'] == 'pikiX6ai') {
        unlink('var/counters.db');
        unlink('var/versions.db');
        echo "local state cleared\n";
    }
    else {
        echo "invalid password\n";
    }
}
else if (isset($_GET['count'])) {
    // Shows total counter of all recorded stacktraces including duplicates
    // Purpose: for local tests of probes
    header('Content-type: text/plain');

    $counter = 0;
    for($stacktrace = dba_firstkey($db_counters); $stacktrace != false; $stacktrace = dba_nextkey($db_counters)) {
        $counter += dba_fetch($stacktrace, $db_counters);
    }
    echo "$counter\n";
}
else {
    header('Content-type: text/plain');
    for($stacktrace = dba_firstkey($db_counters); $stacktrace != false; $stacktrace = dba_nextkey($db_counters)) {
        $counter = dba_fetch($stacktrace, $db_counters);
        $arr[$stacktrace] = $counter;
    }
    // sort array by counter, keys=stacktraces left
    arsort($arr);
    foreach ($arr as $stacktrace => $counter) {
    	$s = rtrim($stacktrace);
        $versions = dba_fetch($stacktrace, $db_versions);
        echo "event counter: $counter\nversions: $versions\n$s\n\n";
    }
}

dba_close($db_counters);

?>
