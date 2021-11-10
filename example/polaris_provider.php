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

$polaris -> InitProvider();

// 实例注册信息
$register_instance_info = array(
	"namespace" => "default",
	"service" => "polaris_php_test",
	"host" => "127.0.0.3",
	"port" => "8080",
	"heartbeat" => "true",
	"protocol" => "gRPC",
	"vpc_id" => "",
	"weight" => "88",
	"ttl" => "3",
	"metadata" => array(
		"client" => "php"
	)
);

// 执行实例注册动作
$res = $polaris->Register($register_instance_info, 5000, 1);
var_dump($res);


// 实例心跳信息
$heartbeat_info = array(
	"namespace" => "default",
	"service" => "polaris_php_test",
	"host" => "127.0.0.3",
	"port" => "8080",
);

// 先进行一次心跳上报，触发实例租约计算任务
$res = $polaris->Heartbeat($heartbeat_info);
var_dump($res);

print "sleep 20 second\n";
// 睡眠等待一段时间，不发实例心跳
sleep(120);

// 执行实例注册动作
$res = $polaris->Heartbeat($heartbeat_info);
var_dump($res);
// 在等待一段时间
sleep(10);


// 实例反注册信息
$deregister_instance_info = array(
	"namespace" => "default",
	"service" => "polaris_php_test",
	"host" => "127.0.0.3",
	"port" => "8080",
);

// 执行实例反注册动作
$res = $polaris->Deregister($deregister_instance_info, 5000, 1);
var_dump($res);
?>