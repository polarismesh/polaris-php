<?php
$br = (php_sapi_name() == "cli")? "":"<br>";

if(!extension_loaded('polaris')) {
	dl('polaris.' . PHP_SHLIB_SUFFIX);
}

// 创建一个 polaris-provider 实例
$polaris = new PolarisClient(array(
	"config_path" => "/root/Github/polaris-php/polaris.yaml",
	"log_dir" => "./"
));

$polaris->InitLimit();

$req_test_3 = array(
	"namespace" => "default",
	"service" => "polaris_php_test",
	"labels" => array(
		"env" => "pre",
		"method" => "GetUserInfo"
	),
);

// FetchRuleLabelKeys
$res = $polaris->GetQuota($req_test_3);
var_dump($res);

$res = $polaris->GetQuota($req_test_3);
var_dump($res);
?>