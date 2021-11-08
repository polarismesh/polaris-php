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
	"service" => "php_ext_test",
	"host" => "127.0.0.3",
	"port" => "8080",
	"metadata" => array(
		"client" => "php"
	)
);

// 执行实例注册动作
$res = $polaris->Register($register_instance_info, 5000, 1);
print $res;
var_dump($register_instance_info);


// 实例心跳


// 实例反注册信息
$deregister_instance_info = array(
	"namespace" => "default",
	"service" => "php_ext_test",
	"host" => "127.0.0.3",
	"port" => "8080",
);

// 执行实例反注册动作
$res = $polaris->Deregister($deregister_instance_info, 5000, 1);
print $res;
var_dump($deregister_instance_info);
?>