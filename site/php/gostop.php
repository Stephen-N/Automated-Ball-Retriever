<?php
$q = strval($_GET['q']);

$process = null;

if ($q == "go") {
    $cmd = "cd ../..; ./website";
    $descriptorspec = array(
        0 => array("pipe", "r"),  // stdin
        1 => array("pipe", "w"),  // stdout -> we use this
        2 => array("pipe", "w")   // stderr 
    );
    $process = proc_open($cmd, $descriptorspec, $pipes);
}

if ($q == "stop") {
    $cmd = "pkill website";
    exec($cmd);
}
