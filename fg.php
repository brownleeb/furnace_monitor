<?php
$handle = fopen("fg1", "r");
echo fread($handle,filesize("fg1"));
fclose ($handle);
$handle = fopen("/user1/hvac/furnace.log","r");
$h="";
#$dt=strtotime(date("now").'- 10 days');
if (empty($_GET["days"])) {
   $days='10'; }
else {
   $days=$_GET["days"];  }
$dt=strtotime(date("now").'- '.$days.' days');
while (! feof($handle))  {
   $d=fgetcsv($handle,60);
   $e=fgetcsv($handle,60);
   $d1=strtotime($d[0].'-'.$d[1].'-'.$d[2]);
   $e1=strtotime($e[0].'-'.$e[1].'-'.$e[2]);
   if ($d1>$dt and $e1>$dt)
       if (gettype($d)=='array' and gettype($e)=='array')
           if (count($d)>0 and count($e)>0)
               $h="        [ '".$d[1]."-".$d[2]."-".$d[0]."', '".$d[6]."', new Date(0,0,0,".$d[3].",".$d[4].",".$d[5]."), new Date(0,0,0,".$e[3].",".$e[4].",".$e[5].")],\n".$h;  }
fclose ($handle);
echo $h;
$handle = fopen("fg2", "r");
echo fread($handle,filesize("fg2"));
fclose ($handle);
$ht=58*($days+1);
echo '<div id="timeline-div" style="height: '.$ht.'px;"></div>';
$handle = fopen("fg3", "r");
echo fread($handle,filesize("fg3"));
fclose ($handle);
?>
