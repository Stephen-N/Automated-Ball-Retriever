<?php
$data = $_POST['data'];
$filename = "gui_info.txt";
echo 'Your ajax post data was ' . $data;

// chmod($filename, 0700); // try also 0666 or 0644
$f = fopen($filename, 'w+');
ftruncate($f, 0);
fwrite($f, $data);
fclose($f);
