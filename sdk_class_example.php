<?php
include "./polaris.class.php";

$polaris_client = new Polaris("./polaris.yaml", "./");

$instance_info = array(
    "namespace" => "default",
    "service" => "php_ext_test",
    "host" => "127.0.0.3",
    "port" => "8080",
    "metadata" => array(
        "env" => "pre"
    ),
    "vpc_id" => "test_vpc",
);

$timeout = 500;
$flow_id = 123456;

$res = $polaris_client->Register($instance_info, $timeout, $flow_id);
var_dump("====================Register==================");
var_dump($res);

$res = $polaris_client->Heartbeat("default", "php_ext_test", "127.0.0.3", "8080", "test_vpc");
var_dump("====================Heartbeat==================");
var_dump($res);


$init_service_info = array(
    "namespace" => "default",
    "service" => "php_ext_test",
    "vpc_id" => "test_vpc",
    "metadata" => array(
        "env" => "pre"
    ),
    "labels" => array(
        "user_id" => "uin_001"
    ),
    "source" => array(
        "namespace" => "",
        "service" => "",
        "metadata" => array(
            "env" => "pre"
        )
    ),
);

$res = $polaris_client->InitService($init_service_info, $timeout, $flow_id);
var_dump("====================InitService==================");
var_dump($res);

$init_service_info = array(
    "namespace" => "default",
    "service" => "php_ext_test",
    "vpc_id" => "test_vpc",
    "metadata" => array(
        "env" => "pre"
    ),
    "labels" => array(
        "user_id" => "uin_001"
    ),
    "source" => array(
        "namespace" => "",
        "service" => "",
        "metadata" => array(
            "env" => "pre"
        )
    )
);

$res = $polaris_client->GetOneInstance($init_service_info, $timeout, $flow_id);
var_dump("====================GetOneInstance==================");
var_dump($res);

$init_service_info = array(
    "namespace" => "default",
    "service" => "php_ext_test",
    "canary" => "test_vpc",
    "source" => array(
        "namespace" => "",
        "service" => "",
        "metadata" => array(
            "env" => "pre"
        )
    )
);

$res = $polaris_client->GetInstances($init_service_info, $timeout, $flow_id);
var_dump("====================GetInstances==================");
var_dump($res);


$init_service_info = array(
    "namespace" => "default",
    "service" => "php_ext_test",
    "canary" => "test_vpc",
    "source" => array(
        "namespace" => "",
        "service" => "",
        "metadata" => array(
            "env" => "pre"
        )
    )
);

$res = $polaris_client->GetAllInstances($init_service_info, $timeout, $flow_id);
var_dump("====================GetAllInstances==================");
var_dump($res);



$init_service_info = array(
    "namespace" => "default",
    "service" => "php_ext_test",
    "locality_aware_info" => "",
    "ret_status" => "ok",
    "host" => "127.0.0.1",
    "port" => "8080",
);

$res = $polaris_client->UpdateServiceCallResult($init_service_info, $timeout, $flow_id);
var_dump("====================UpdateServiceCallResult==================");
var_dump($res);

$res = $polaris_client->GetQuota("default", "php_ext_test", 0, array(), array());
var_dump("====================GetQuota==================");
var_dump($res);

$res = $polaris_client->UpdateCallResult("default", "php_ext_test", "100", "100", "100", array(), array());
var_dump("====================UpdateCallResult==================");
var_dump($res);

$res = $polaris_client->InitQuotaWindow("default", "php_ext_test", 0, array(), array());
var_dump("====================InitQuotaWindow==================");
var_dump($res);


$instance_info = array(
    "namespace" => "default",
    "service" => "php_ext_test",
    "host" => "127.0.0.3",
    "port" => "8080",
    "vpc_id" => "test_vpc",
);

$res = $polaris_client->Deregister($instance_info, $timeout, $flow_id);
var_dump("====================Deregister==================");
var_dump($res);