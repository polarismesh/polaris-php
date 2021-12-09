<?php
$br = (php_sapi_name() == "cli")? "":"<br>";

if(!extension_loaded('polaris')) {
	dl('polaris.' . PHP_SHLIB_SUFFIX);
}

// 创建一个 polaris-provider 实例
$polaris = new PolarisClient(array(
	"config_path" => "./polaris.yaml",
	"log_dir" => "./"
));

$polaris->InitConsumer();

$client_req = array(
	"namespace" => "default",
	"service" => "polaris_php_test",
	"labels" => array(
		"env" => "pre",
	),
);

$res = $polaris->GetOneInstance($get_req, 5000, 1);
$instances = $res["response"]["instances"];
?>