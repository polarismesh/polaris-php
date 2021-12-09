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
$client_req = array(
	"namespace" => "default",
	"service" => "polaris-php-provider",
);

$polaris->InitConsumer();

$res = $polaris->GetInstances($client_req, 5000, 1);
var_dump($res);

for ($i=1; $i<=10; $i++)
{
    $call_service_result = array(
        "namespace" => "default",
        "service" => "polaris-php-provider",
        "host" => "127.0.0.3",
        "port" => "8080",
        "ret_status" => "error",
    );
    
    $timeout = 500;
    $flow_id = 123456;
    $res = $polaris->UpdateServiceCallResult($call_service_result, $timeout, $flow_id);
    var_dump($res);
    sleep(1);
}

$res = $polaris->GetInstances($client_req, 5000, 1);
$instances = $res["response"]["instances"];
var_dump($instances);
?>