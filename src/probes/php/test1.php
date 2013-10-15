<?php

require("randomtest-probe.php");

function fun_A($arr) {
    return $arr[10];
}

function fun_C() {
    $arr = $undefined_var;
}

function fun_B() {
    $arr = array( 2, 3, 4 );
    fun_A($arr);
    fun_C();
}

fun_B();
fun_C();

?>
