<?php

if(!extension_loaded('polaris')) {
	dl('polaris.' . PHP_SHLIB_SUFFIX);
}
if(!extension_loaded('pcntl')) {
	dl('pcntl.' . PHP_SHLIB_SUFFIX);
}

set_time_limit(0);


$self_ip  = getenv('PHP_PROVIDER_IP');
var_dump($self_ip);

class TCPServer
{
    private $port = 9996;

    private $_socket = null;

    public function __construct()
    {
        $this->_socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
        if ($this->_socket === false) {
            die(socket_strerror(socket_last_error($this->_socket)));
        }
    }

    public function run()
    {
        $bind_ok = socket_bind($this->_socket, '0.0.0.0', $this->port);

        if ($bind_ok === false) {
            echo "socket_bind() failed, reason: ".socket_strerror(socket_last_error())."\n";
        } else {
            echo "socket_bind() success";
        }

        socket_listen($this->_socket, 5);
        while(true) {
            $socketAccept = socket_accept($this->_socket);
            $request = socket_read($socketAccept, 1024);
            echo "receive from client '$request'";
            socket_write($socketAccept, 'hello. I`m provider', strlen('hello. I`m provider'));
            socket_close($socketAccept);
        }
    }

    public function close()
    {
        socket_close($this->_socket);
    }
}


$pid = pcntl_fork();
if ( $pid == 0 ) {
    $self_ip  = getenv('PHP_PROVIDER_IP');
    var_dump($self_ip);

    // 创建一个 polaris-provider 实例
    $polaris = new PolarisClient(array(
    	"config_path" => "./polaris.yaml",
    	"log_dir" => "./"
    ));

    $polaris->InitProvider();

    // 实例注册信息
    $register_instance_info = array(
    	"namespace" => "default",
    	"service" => "polaris_php_test",
    	"host" => $self_ip,
    	"port" => "9996",
    	"heartbeat" => "true",
    	"protocol" => "TCP",
    	"vpc_id" => "",
    	"weight" => "88",
    	"ttl" => "5",
    	"metadata" => array(
    		"client" => "php"
    	)
    );

    // 执行实例注册动作
    $res = $polaris->Register($register_instance_info, 5000, 1);
    var_dump($res);
    if ( $res['code'] != '0') {
        $err_msg = $res["err_msg"];
        echo "register instance fail '$err_msg'";
        exit(1);
    }

    // 实例心跳信息
    $heartbeat_info = array(
    	"namespace" => "default",
    	"service" => "polaris_php_test",
    	"host" => $self_ip,
    	"port" => "9996",
    );
    while (true) {
        // 先进行一次心跳上报，触发实例租约计算任务
        $res = $polaris->Heartbeat($heartbeat_info);
        sleep(4);
        var_dump($res);
    }
} else {
    echo "run tpc server\n";
    $tcp_server = new TCPServer();
    $tcp_server->run();
}
?>