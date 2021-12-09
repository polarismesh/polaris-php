<?php
$br = (php_sapi_name() == "cli")? "":"<br>";

if(!extension_loaded('polaris')) {
	dl('polaris.' . PHP_SHLIB_SUFFIX);
}

// 创建一个 polaris-client 实例
$polaris = new PolarisClient(array(
	"config_path" => "./polaris.yaml",
	"log_dir" => "./"
));

$polaris -> InitConsumer();

$get_req = array(
	"namespace" => "default",
	"service" => "polaris_php_test",
);

$res = $polaris->GetOneInstance($get_req, 5000, 1);

$instances = $res["response"]["instances"];

$address = $instances[0]['host'];
$service_port = (int)$instances[0]["port"];

// 创建并返回一个套接字（通讯节点）
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if ($socket === false) {
    echo "socket_create() failed, reason: ".socket_strerror(socket_last_error())."\n";
} 

echo "Attempting to connect to '$address' on port '$service_port'...";
// 发起socket连接请求
$result = socket_connect($socket, $address, $service_port);
if($result === false) {
    echo "socket_connect() failed, reason: ".socket_strerror(socket_last_error($socket))."\n";
    exit(1);
} else {
    echo "Connect success. \n";
}

$input = "This is a message from client"."\n";

// 向socket服务器发送消息
socket_write($socket, $input, strlen($input));
echo  "Client send success \n";

echo "Reading response:\n";
// 读取socket服务器发送的消息
while ($out = socket_read($socket, 8192)) {
    echo $out;
}
echo PHP_EOL;
socket_close($socket); // 关闭socket连接

?>