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

$polaris->InitLimit();

$client_req = array(
	"namespace" => "default",
	"service" => "polaris_php_test",
	"labels" => array(
		"env" => "pre",
		"method" => "GetUserInfo"
	),
);

// FetchRuleLabelKeys
$res = $polaris->GetQuota($client_req);
var_dump($res);

$res = $polaris->GetQuota($client_req);
var_dump($res);
?>