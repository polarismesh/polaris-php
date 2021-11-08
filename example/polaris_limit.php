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

$req_test_1 = array(
	"namespace" => "default",
	"service" => "polaris_php_test",
);

// FetchRuleLabelKeys
$res = $polaris->FetchRule($req_test_1, 1000);
var_dump($req_test_1);
var_dump($res);

$req_test_2 = array(
	"namespace" => "default",
	"service" => "polaris_php_test",
);

// FetchRuleLabelKeys
$res = $polaris->FetchRuleLabelKeys($req_test_2, 1000);

var_dump($req_test_1);
var_dump($res);

$req_test_3 = array(
	"namespace" => "default",
	"service" => "polaris_php_test",
	"limit_labels" => array(
		"env" => "pre"
	),
	"subset" => array(
		"set_1" => "cluster_1"
	)
);

// FetchRuleLabelKeys
$res = $polaris->GetQuota($req_test_3);

var_dump($req_test_3);
var_dump($res);
?>