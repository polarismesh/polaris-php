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