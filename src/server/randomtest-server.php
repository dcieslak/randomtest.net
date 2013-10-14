<?php

header('Content-type: text/plain');

echo "OK\n";
echo $_POST['stacktrace'];

file_put_contents('/tmp/randomtest.log', $_POST['stacktrace']);

?>
