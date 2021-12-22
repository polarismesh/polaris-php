 <!-- Tencent is pleased to support the open source community by making polaris-go available.

 Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.

 Licensed under the BSD 3-Clause License (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 https://opensource.org/licenses/BSD-3-Clause

 Unless required by applicable law or agreed to in writing, software distributed
 under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 CONDITIONS OF ANY KIND, either express or implied. See the License for the
 specific language governing permissionsr and limitations under the License. -->
 
<?php

// htt-server demo is copy from : https://segmentfault.com/a/1190000007938431

$br = (php_sapi_name() == "cli")? "":"<br>";

if(!extension_loaded('polaris')) {
	dl('polaris.' . PHP_SHLIB_SUFFIX);
}

set_time_limit(0);

class HttpServer
{
    private $ip = '127.0.0.1';
    private $port = 18090;

    private $_socket = null;
    // 创建一个 polaris-client 实例
    private $polaris = null;

    public function __construct()
    {
        $this->_socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
        if ($this->_socket === false) {
            die(socket_strerror(socket_last_error($this->_socket)));
        }
        $this->$polaris = new PolarisClient(array(
            "config_path" => "./polaris.yaml",
            "log_dir" => "./"
        ));
        $this->$polaris->InitConsumer();
        
    }

    public function run()
    {
        socket_bind($this->_socket, '0.0.0.0', $this->port);
        socket_listen($this->_socket, 16);
        while(true) {
            $socketAccept = socket_accept($this->_socket);
            $request = socket_read($socketAccept, 1024);
            echo $request;
            socket_write($socketAccept, 'HTTP/1.1 200 OK'.PHP_EOL);
            socket_write($socketAccept, 'Date:'.date('Y-m-d H:i:s').PHP_EOL);

            $fileName = $this->getUri($request);
            
            $get_req = array(
            	"namespace" => "default",
            	"service" => "EchoServerPHP",
            );
            $res = $this->$polaris->GetOneInstance($get_req, 5000, 1);
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

            //set content type
            socket_write($socketAccept, 'Content-Type: text/html'.PHP_EOL);
            socket_write($socketAccept, ''.PHP_EOL);
            // 读取socket服务器发送的消息
            while ($out = socket_read($socket, 8192)) {\
                socket_write($socketAccept, $out, strlen($out));
                echo $out;
            }
            socket_close($socket); // 关闭socket连接
            socket_close($socketAccept);

        }

    }

    protected function getUri($request = '')
    {
        $arrayRequest = explode(PHP_EOL, $request);
        $line = $arrayRequest[0];
        $file = trim(preg_replace('/(\w+)\s\/(.*)\sHTTP\/1.1/i','$2', $line));
        return $file;
    }


    public function close()
    {
        socket_close($this->_socket);
    }

}
$httpServer = new HttpServer();

echo "run http-server";
$httpServer->run();

?>