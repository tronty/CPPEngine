<?php
$count = file_get_contents("countlog.txt");
$count = trim($count);
$count = $count + 1;
$fl = fopen("countlog.txt","w+");
fwrite($fl,$count);
fclose($fl);
?>

