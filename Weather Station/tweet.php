<?php
$temperature = file_get_contents('http://thingspeak.com/channels/xxx/field/1/last.html');
$pressure = file_get_contents('http://thingspeak.com/channels/xxx/field/2/last.html');
$humidity = file_get_contents('http://thingspeak.com/channels/xxx/field/4/last.html');
require_once ('codebird.php');
\Codebird\Codebird::setConsumerKey('xxx', 'xxx');
$cb = \Codebird\Codebird::getInstance();
$cb->setToken('xxx', 'xxx');

$tweet = array(
      'status' => 'Temperature:'.$temperature.'C, Pressure:'.$pressure.'atm, Humidity:'.$humidity.'%'
    );

$cb->statuses_update($tweet);
?>
