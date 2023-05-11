<?php
$fp = fopen('test.txt', "a+");
chmod("test.txt", 0777); // try also 0666 or 0644
fwrite($fp, 'Cats chase mice');
fclose($fp);
?>