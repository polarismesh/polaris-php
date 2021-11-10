<?php
$br = (php_sapi_name() == "cli")? "":"<br>";

if(!extension_loaded('polaris')) {
	dl('polaris.' . PHP_SHLIB_SUFFIX);
}

// 创建一个 polaris-client 实例
$polaris = new PolarisClient(array(
	"config_path" => "/root/Github/polaris-php/polaris.yaml",
	"log_dir" => "./"
));

$polaris -> InitConsumer();

$get_req = array(
	"namespace" => "default",
	"service" => "polaris_consumer_test",

);

$res = $polaris->GetInstances($get_req, 5000, 1);
var_dump($res);

?>