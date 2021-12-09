# Polaris-PHP SDK 方法使用文档


## PolarisClient

### new PolarisClient(array())

> 方法用途

`PolarisClient`的构造函数，用于创建一个`PolarisClient`实例进行使用

> 参数描述

*函数入参*

| 参数名称      | 参数类型 | 参数用途                        |
| :------------ | :------- | :------------------------------ |
| `config_path` | string   | 设置配置文件的文件路径信息      |
| `log_dir`     | string   | 设置SDK自身的日志文件保存的位置 |

> 具体代码使用示例

```php
$param = array(
    "config_path" => "./polaris.yaml",
    "log_dir" => "./"
);

$polaris_client = new PolarisClient($param)
```

### InitProvider

> 方法用途

用于开启`PolarisClient`的`ProviderAPI`的相关能力

- 注册服务实例
- 反注册实例子
- 上报服务实例心跳

> 参数描述

该方法无入参数

> 具体使用示例

```php
$param = array(
    "config_path" => "./polaris.yaml",
    "log_dir" => "./"
);

$polaris_client = new PolarisClient($param)

$polaris_client->InitProvider();
```

### InitConsumer

> 方法用途

用于开启`PolarisClient`的`ConsumerAPI`的相关能力

- 获取单个服务的一个实例
- 获取单个服务的实例
- 上报服务调用信息
- ...

> 参数描述

该方法无入参数

> 具体代码使用示例

```php
$param = array(
    "config_path" => "./polaris.yaml",
    "log_dir" => "./"
);

$polaris_client = new PolarisClient($param);

$polaris_client->InitConsumer();
```

### InitLimit

> 方法用途

用于开启`PolarisClient`的`LimitAPI`的相关能力

- 获取限流规则的配额信息
- 查看当前限流结果
- 更新请求配额调用结果
- ...

> 参数描述

该方法无入参数

> 具体代码使用示例

```php
$param = array(
    "config_path" => "./polaris.yaml",
    "log_dir" => "./"
);

$polaris_client = new PolarisClient($param);

$polaris_client->InitLimit();
```

***

## ProviderAPI

### Register

> 方法用途

注册一个服务实例到北极星

> 参数描述

*函数入参*

| 一级参数名称 | 二级参数名称 | 参数类型            | 参数用途                                                                     |
| :----------- | :----------- | :------------------ | :--------------------------------------------------------------------------- |
| `timeout`    |              | long                | 超时时间，单位毫秒                                                           |
| `flowId`     |              | long                | 本次请求的标识ID                                                             |
| `instance`   |              | map<string, object> |                                                                              |
| ├            | `namespace`  | string              | 服务所在的命名空间                                                           |
| ├            | `service`    | string              | 服务名称                                                                     |
| ├            | `host`       | string              | 实例的IP                                                                     |
| ├            | `port`       | string              | 实例的端口，字符串数字: "100"                                                |
| ├            | `protocol`   | string              | 实例端口的协议                                                               |
| ├            | `priority`   | string              | 实例优先级，字符串数字: "100"                                                |
| ├            | `weight`     | string              | 实例的权重，字符串数字: "100"                                                |
| ├            | `version`    | string              | 实例版本                                                                     |
| ├            | `vpc_id`     | string              | 实例所在的VPC_ID信息                                                         |
| ├            | `heartbeat`  | bool                | 是否开启心服务端对本实例执行跳健康检查                                       |
| ├            | `ttl`        | string              | 心跳间隔时间，单位为秒，仅在 `heartbeat` 为 `true` 下生效，字符串数字: "100" |
| ⎿            | `metadata`   | map<string, string> | 实例的标签信息                                                               |



*函数执行返回*

| 参数名称      | 参数类型 | 参数用途                   | 示例 |
| :------------ | :------- | :------------------------- | :--- |
| `code`        | int      | 方法执行状态码             |      |
| `err_msg`     | string   | 错误信息                   |      |
| `instance_id` | string   | 注册成功，实例的唯一ID标识 |      |

> 具体代码使用示例

```php
$polaris_client = new PolarisClient(array());

// 初始化服务提供者的能力
$polaris_client->InitProvider();

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
var_dump($res);
```

### Deregister

> 方法用途

提供实例反注册功能，即主动将该实例从北极星中彻底删除

> 参数描述

*函数入参*

| 一级参数名称 | 二级参数名称 | 参数类型            | 参数用途                      |
| :----------- | :----------- | :------------------ | :---------------------------- |
| `timeout`    |              | long                | 超时时间，单位毫秒            |
| `flowId`     |              | long                | 本次请求的标识ID              |
| instance     |              | map<string, object> |                               |
| ├            | `namespace`  | string              | 服务所在的命名空间            |
| ├            | `service`    | string              | 服务名称                      |
| ├            | `host`       | string              | 实例的IP                      |
| ├            | `port`       | string              | 实例的端口，字符串数字: "100" |
| ⎿            | `vpc_id`     | string              | 实例所在的VPC_ID信息          |


*函数执行返回*

| 参数名称  | 参数类型 | 参数用途       |
| :-------- | :------- | :------------- |
| `code`    | int      | 方法执行状态码 |
| `err_msg` | string   | 错误信息       |

> 具体代码使用示例

```php
$polaris_client = new PolarisClient(array());

// 初始化服务提供者的能力
$polaris_client->InitProvider();

$instance_info = array(
	"namespace" => "default",
	"service" => "php_ext_test",
	"host" => "127.0.0.3",
	"port" => "8080",
    "vpc_id" => "test_vpc",
);

$timeout = 500;
$flow_id = 123456;

$res = $polaris_client->Deregister($instance_info, $timeout, $flow_id);
var_dump($res);
```

### Heartbeat

> 方法用途

提供上报实例心跳功能

> 参数描述

*函数入参*

| 参数名称    | 参数类型 | 参数用途                      |
| :---------- | :------- | :---------------------------- |
| `namespace` | string   | 服务所在的命名空间            |
| `service`   | string   | 服务名称                      |
| `host`      | string   | 实例的IP                      |
| `port`      | string   | 实例的端口，字符串数字: "100" |
| `vpc_id`    | string   | 实例所在的VPC_ID信息          |


*函数执行返回*

| 参数名称  | 参数类型 | 参数用途       |
| :-------- | :------- | :------------- |
| `code`    | int      | 方法执行状态码 |
| `err_msg` | string   | 错误信息       |

> 具体代码使用示例

```php
$polaris_client = new PolarisClient(array());

// 初始化服务提供者的能力
$polaris_client->InitProvider();

$heartbeat_info = array(
	"namespace" => "default",
	"service" => "php_ext_test",
	"host" => "127.0.0.3",
	"port" => "8080",
    "vpc_id" => "test_vpc",
);


$res = $polaris_client->Heartbeat($heartbeat_info);
var_dump($res);
```

***

## ConsmerAPI

### 前提

| Key                      | Value类别      | 作用                              |
| :----------------------- | :------------- | :-------------------------------- |
| `load_balance_type`      | weightedRandom | 权重随机                          |
|                          | ringHash       | 一致性hash负载均衡                |
|                          | maglev         | 一致性Hash: maglev 算法           |
|                          | l5cst          | 兼容L5的一致性Hash                |
|                          | simpleHash     | hash_key%总实例数 选择服务实例    |
|                          | cMurmurHash    | 兼容brpc c_murmur的一致性哈希     |
|                          | localityAware  | 兼容brpc locality_aware的负载均衡 |
|                          | default        | 使用全局配置的负载均衡算法        |
|                          |                |                                   |
| `metadata_failover_type` | notKey         | 返回不包含元数据路由key的节点     |
|                          | all            | 降级返回所有节点                  |
|                          | none           | 默认不降级                        |
|                          |                |                                   |
| `weight_type`            | static         | 静态权重                          |
|                          | dynamic        | 动态权重                          |
|                          |                |                                   |
| `ret_status`             | error          | 服务实例错误                      |
|                          | timeout        | 服务实例超时                      |
|                          | ok             | 服务实例正常                      |

### InitService

> 方法用途

用于提前初始化服务数据

> 参数描述

*请求参数*

| 一级参数名称 | 二级参数名称             | 三级参数名称 | 参数类型            | 参数用途                                             |
| :----------- | :----------------------- | :----------- | :------------------ | :--------------------------------------------------- |
| `timeout`    |                          |              | long                | 超时时间，单位毫秒                                   |
| `flowId`     |                          |              | long                | 本次请求的标识ID                                     |
| `instance`   |                          |              | map<string, object> |                                                      |
| ├            | `namespace`              |              | string              | 被调服务所在的命名空间                               |
| ├            | `service`                |              | string              | 被调服务名称                                         |
| ├            | `version`                |              | string              | 实例版本                                             |
| ├            | `vpc_id`                 |              | string              | 实例所在的VPC_ID信息                                 |
| ├            | `metadata`               |              | map<string, string> | 设置元数据，用于元数据路由                           |
| ├            | `canary`                 |              | string              | 设置调用哪个金丝雀服务实例                           |
| ├            | `source_set_name`        |              | string              | 设置调用哪个set下的服务                              |
| ├            | `ignore_half_open`       |              | bool                | 设置是否略过跳过半开探测节点                         |
| ├            | `hash_string`            |              | string              | 设置 hash 字符串，用于一致性哈希负载均衡算法         |
| ├            | `hash_key`               |              | long                | 设置hash key，用于一致性哈希负载均衡算法             |
| ├            | `replicate_index`        |              | long                | 用于一致性hash算法时获取副本实例                     |
| ├            | `backup_instance_num`    |              | long                | 设置用于重试的实例数。可选，默认不返回用于重试的实例 |
| ├            | `load_balance_type`      |              | string              | 设置负载均衡类型。可选，默认使用配置文件中设置的类型 |
| ├            | `metadata_failover_type` |              | string              | 设置元数据路由匹配失败时的降级策略，默认不降级       |
| ├            | `labels`                 |              | map<string, string> | 设置请求标签，用于接口级别熔断                       |
| ⎿            | `source`                 |              | map<string, object> | 设置源服务信息，用于服务路由计算。可选               |
|              | ├                        | `namespace`  | string              | 实例的标签信息                                       |
|              | ├                        | `service`    | string              | 实例的标签信息                                       |
|              | ⎿                        | `metadata`   | map<string, string> | 实例的标签信息                                       |


*函数执行返回*

| 参数名称  | 参数类型 | 参数用途       | 示例 |
| :-------- | :------- | :------------- | :--- |
| `code`    | int      | 方法执行状态码 |      |
| `err_msg` | string   | 错误信息       |      |

> 具体代码使用示例

```php
$polaris_client = new PolarisClient(array());

// 初始化服务提供者的能力
$polaris_client->InitConsumer();

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

$timeout = 500;
$flow_id = 123456;

$res = $polaris_client->InitService($init_service_info, $timeout, $flow_id);
var_dump($res);
```


### GetOneInstance


> 方法用途

用于提前初始化服务数据

> 参数描述

*请求参数*

| 一级参数名称 | 二级参数名称             | 三级参数名称 | 参数类型            | 参数用途                                                        |
| :----------- | :----------------------- | :----------- | :------------------ | :-------------------------------------------------------------- |
| `timeout`    |                          |              | long                | 超时时间，单位毫秒                                              |
| `flowId`     |                          |              | long                | 本次请求的标识ID                                                |
| `instance`   |                          |              | map<string, object> |                                                                 |
| ├            | `namespace`              |              | string              | 被调服务所在的命名空间                                          |
| ├            | `service`                |              | string              | 被调服务名称                                                    |
| ├            | `version`                |              | string              | 实例版本                                                        |
| ├            | `vpc_id`                 |              | string              | 实例所在的VPC_ID信息                                            |
| ├            | `metadata`               |              | map<string, string> | 设置元数据，用于元数据路由                                      |
| ├            | `canary`                 |              | string              | 设置调用哪个金丝雀服务实例                                      |
| ├            | `source_set_name`        |              | string              | 设置调用哪个set下的服务                                         |
| ├            | `ignore_half_open`       |              | string              | 设置是否略过跳过半开探测节点, "true" or "false"                 |
| ├            | `hash_string`            |              | string              | 设置 hash 字符串，用于一致性哈希负载均衡算法，字符串数字: "100" |
| ├            | `hash_key`               |              | string              | 设置hash key，用于一致性哈希负载均衡算法，字符串数字: "100"     |
| ├            | `replicate_index`        |              | string              | 用于一致性hash算法时获取副本实例，字符串数字: "100"             |
| ├            | `backup_instance_num`    |              | string              | 设置用于重试的实例数。可选，默认不返回用于重试的实例            |
| ├            | `load_balance_type`      |              | string              | 设置负载均衡类型。可选，默认使用配置文件中设置的类型            |
| ├            | `metadata_failover_type` |              | string              | 设置元数据路由匹配失败时的降级策略，默认不降级                  |
| ├            | `labels`                 |              | map<string, string> | 设置请求标签，用于接口级别熔断                                  |
| ⎿            | `source`                 |              | map<string, object> | 设置源服务信息，用于服务路由计算。可选                          |
|              | ├                        | `namespace`  | string              | 实例的标签信息                                                  |
|              | ├                        | `service`    | string              | 实例的标签信息                                                  |
|              | ⎿                        | `metadata`   | map<string, string> | 实例的标签信息                                                  |

*函数执行返回*

| 一级参数名称 | 二级参数名称  | 三级参数名称          | 参数类型            | 参数用途                 |
| :----------- | :------------ | :-------------------- | :------------------ | :----------------------- |
| `code`       |               |                       | int                 | 方法执行状态码           |
| `err_msg`    |               |                       | string              | 错误信息                 |
| `response`   |               |                       | map<string, object> | InstancesResponse        |
| ├            | `flow_id`     |                       | long                | 请求流水号               |
| ├            | `service`     |                       | string              | 服务名                   |
| ├            | `namespace`   |                       | string              | 命名空间                 |
| ├            | `revision`    |                       | string              | 版本信息                 |
| ├            | `metadata`    |                       | map<string, string> | 服务元数据               |
| ├            | `weight_type` |                       | string              | 权重类型                 |
| ├            | `subset`      |                       | map<string, string> | 实例所属的subset         |
| ⎿            | `instances`   |                       | map<string, object> | 服务实例列表             |
|              | ├             | `instance_id`         | string              | 服务实例ID               |
|              | ├             | `host`                | string              | 服务的节点IP或者域名     |
|              | ├             | `port`                | int                 | 节点端口号               |
|              | ├             | `container_name`      | string              | 实例元数据信息中的容器名 |
|              | ├             | `internal_set_name`   | string              | 实例元数据信息中的set名  |
|              | ├             | `logic_set`           | string              | 实例LogicSet信息         |
|              | ├             | `region`              | string              | location region          |
|              | ├             | `zone`                | string              | location zone            |
|              | ├             | `campus`              | string              | location campus          |
|              | ├             | `vpc_id`              | string              | 获取服务实例所在VPC ID   |
|              | ├             | `protocol`            | string              | 实例协议信息             |
|              | ├             | `version`             | int                 | 实例版本号信息           |
|              | ├             | `weight`              | int                 | 实例静态权重值, 0-1000   |
|              | ├             | `priority`            | int                 | 实例优先级               |
|              | ├             | `dynamic_weight`      | int                 | 实例动态权重             |
|              | ├             | `hash_key`            | int                 | GetHash                  |
|              | ├             | `locality_aware_info` | int                 | locality_aware_info      |
|              | ├             | `healthy`             | bool                | 实例健康状态             |
|              | ├             | `isola`               | bool                | 实例隔离状态             |
|              | ⎿             | `metadata`            | map<string, string> | 实例元数据信息           |

> 具体代码使用示例

```php
$polaris_client = new PolarisClient(array());

// 初始化服务提供者的能力
$polaris_client->InitConsumer();

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

$timeout = 500;
$flow_id = 123456;

$res = $polaris_client->GetOneInstance($init_service_info, $timeout, $flow_id);
var_dump($res);
/*

$res的结构

array(
    "code" => 0,
    "err_msg" => ,
    "response" => array(
        "flow_id"  =>
        "service"  =>
        "namespace"  =>
        "revision"  =>
        "metadata"  =>
        "weight_type"  =>
        "subset"  =>
        "instances"  => array(
            "instance_id" =>
            "host" =>
            "port" =>
            "container_name" =>
            "internal_set_name" =>
            "logic_set" =>
            "region" =>
            "zone" =>
            "campus" =>
            "vpc_id" =>
            "protocol" =>
            "version" =>
            "weight" =>
            "priority" =>
            "dynamic_weight" =>
            "hash_key" =>
            "locality_aware_info" =>
            "healthy" =>
            "isola" =>
            "metadata" => array(
                "{key}" => "{value}" 
            )
        )
    )
)

 */
```

### GetInstances

> 方法用途

用于提前初始化服务数据

> 参数描述

*请求参数*

| 参数名称   |                          |             | 参数类型            | 参数用途                                                                |
| :--------- | :----------------------- | :---------- | :------------------ | :---------------------------------------------------------------------- |
| `timeout`  |                          |             | long                | 超时时间，单位毫秒                                                      |
| `flowId`   |                          |             | long                | 本次请求的标识ID                                                        |
| `instance` |                          |             | map<string, object> |                                                                         |
| ├          | `namespace`              |             | string              | 被调服务所在的命名空间                                                  |
| ├          | `service`                |             | string              | 被调服务名称                                                            |
| ├          | `version`                |             | string              | 实例版本                                                                |
| ├          | `vpc_id`                 |             | string              | 实例所在的VPC_ID信息                                                    |
| ├          | `metadata`               |             | map<string, string> | 设置元数据，用于元数据路由                                              |
| ├          | `canary`                 |             | string              | 设置调用哪个金丝雀服务实例                                              |
| ├          | `source_set_name`        |             | string              | 设置调用哪个set下的服务                                                 |
| ├          | `ignore_half_open`       |             | string              | 设置是否略过跳过半开探测节点, "true" or "false"                         |
| ├          | `hash_string`            |             | string              | 设置 hash 字符串，用于一致性哈希负载均衡算法                            |
| ├          | `hash_key`               |             | string              | 设置hash key，用于一致性哈希负载均衡算法，字符串数字: "100"             |
| ├          | `replicate_index`        |             | string              | 用于一致性hash算法时获取副本实例，字符串数字: "100"                     |
| ├          | `backup_instance_num`    |             | string              | 设置用于重试的实例数。可选，默认不返回用于重试的实例，字符串数字: "100" |
| ├          | `load_balance_type`      |             | string              | 设置负载均衡类型。可选，默认使用配置文件中设置的类型                    |
| ├          | `metadata_failover_type` |             | string              | 设置元数据路由匹配失败时的降级策略，默认不降级                          |
| ├          | `labels`                 |             | map<string, string> | 设置请求标签，用于接口级别熔断                                          |
| ⎿          | `source`                 |             | map<string, string> | 设置源服务信息，用于服务路由计算。可选                                  |
|            | ├                        | `namespace` | string              | 实例的标签信息                                                          |
|            | ├                        | `service`   | string              | 实例的标签信息                                                          |
|            | ⎿                        | `metadata`  | map<string, string> | 实例的标签信息                                                          |



*函数执行返回*

| 一级参数名称 | 二级参数名称  | 三级参数名称          | 参数类型            | 参数用途                 |
| :----------- | :------------ | :-------------------- | :------------------ | :----------------------- |
| `code`       |               |                       | int                 | 方法执行状态码           |
| `err_msg`    |               |                       | string              | 错误信息                 |
| `response`   |               |                       | map<string, object> | InstancesResponse        |
| ├            | `flow_id`     |                       | long                | 请求流水号               |
| ├            | `service`     |                       | string              | 服务名                   |
| ├            | `namespace`   |                       | string              | 命名空间                 |
| ├            | `revision`    |                       | string              | 版本信息                 |
| ├            | `metadata`    |                       | map<string, string> | 服务元数据               |
| ├            | `weight_type` |                       | string              | 权重类型                 |
| ├            | `subset`      |                       | map<string, string> | 实例所属的subset         |
| ⎿            | `instances`   |                       | map<string, object> | 服务实例列表             |
|              | ├             | `instance_id`         | string              | 服务实例ID               |
|              | ├             | `host`                | string              | 服务的节点IP或者域名     |
|              | ├             | `port`                | int                 | 节点端口号               |
|              | ├             | `container_name`      | string              | 实例元数据信息中的容器名 |
|              | ├             | `internal_set_name`   | string              | 实例元数据信息中的set名  |
|              | ├             | `logic_set`           | string              | 实例LogicSet信息         |
|              | ├             | `region`              | string              | location region          |
|              | ├             | `zone`                | string              | location zone            |
|              | ├             | `campus`              | string              | location campus          |
|              | ├             | `vpc_id`              | string              | 获取服务实例所在VPC ID   |
|              | ├             | `protocol`            | string              | 实例协议信息             |
|              | ├             | `version`             | int                 | 实例版本号信息           |
|              | ├             | `weight`              | int                 | 实例静态权重值, 0-1000   |
|              | ├             | `priority`            | int                 | 实例优先级               |
|              | ├             | `dynamic_weight`      | int                 | 实例动态权重             |
|              | ├             | `hash_key`            | int                 | GetHash                  |
|              | ├             | `locality_aware_info` | int                 | locality_aware_info      |
|              | ├             | `healthy`             | bool                | 实例健康状态             |
|              | ├             | `isola`               | bool                | 实例隔离状态             |
|              | ⎿             | `metadata`            | map<string, string> | 实例元数据信息           |

> 具体代码使用示例

```php
$polaris_client = new PolarisClient(array());

// 初始化服务提供者的能力
$polaris_client->InitConsumer();

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

$timeout = 500;
$flow_id = 123456;

$res = $polaris_client->GetInstances($init_service_info, $timeout, $flow_id);
var_dump($res);
```

### GetAllInstances


> 方法用途

用于提前初始化服务数据

> 参数描述

*请求参数*

| 参数名称   |                          |             | 参数类型            | 参数用途                                                                |
| :--------- | :----------------------- | :---------- | :------------------ | :---------------------------------------------------------------------- |
| `timeout`  |                          |             | long                | 超时时间，单位毫秒                                                      |
| `flowId`   |                          |             | long                | 本次请求的标识ID                                                        |
| `instance` |                          |             | map<string, object> |                                                                         |
| ├          | `namespace`              |             | string              | 被调服务所在的命名空间                                                  |
| ├          | `service`                |             | string              | 被调服务名称                                                            |
| ├          | `version`                |             | string              | 实例版本                                                                |
| ├          | `vpc_id`                 |             | string              | 实例所在的VPC_ID信息                                                    |
| ├          | `metadata`               |             | map<string, string> | 设置元数据，用于元数据路由                                              |
| ├          | `canary`                 |             | string              | 设置调用哪个金丝雀服务实例                                              |
| ├          | `source_set_name`        |             | string              | 设置调用哪个set下的服务                                                 |
| ├          | `ignore_half_open`       |             | string              | 设置是否略过跳过半开探测节点, "trur" or "false"                         |
| ├          | `hash_string`            |             | string              | 设置 hash 字符串，用于一致性哈希负载均衡算法，字符串数字: "100"         |
| ├          | `hash_key`               |             | string              | 设置hash key，用于一致性哈希负载均衡算法                                |
| ├          | `replicate_index`        |             | string              | 用于一致性hash算法时获取副本实例，字符串数字: "100"                     |
| ├          | `backup_instance_num`    |             | string              | 设置用于重试的实例数。可选，默认不返回用于重试的实例，字符串数字: "100" |
| ├          | `load_balance_type`      |             | string              | 设置负载均衡类型。可选，默认使用配置文件中设置的类型                    |
| ├          | `metadata_failover_type` |             | string              | 设置元数据路由匹配失败时的降级策略，默认不降级                          |
| ├          | `labels`                 |             | map<string, string> | 设置请求标签，用于接口级别熔断                                          |
| ⎿          | `source`                 |             | map<string, string> | 设置源服务信息，用于服务路由计算。可选                                  |
|            | ├                        | `namespace` | string              | 实例的标签信息                                                          |
|            | ├                        | `service`   | string              | 实例的标签信息                                                          |
|            | ⎿                        | `metadata`  | map<string, string> | 实例的标签信息                                                          |



*函数执行返回*

| 一级参数名称 | 二级参数名称  | 三级参数名称          | 参数类型            | 参数用途                 |
| :----------- | :------------ | :-------------------- | :------------------ | :----------------------- |
| `code`       |               |                       | int                 | 方法执行状态码           |
| `err_msg`    |               |                       | string              | 错误信息                 |
| `response`   |               |                       | map<string, object> | InstancesResponse        |
| ├            | `flow_id`     |                       | long                | 请求流水号               |
| ├            | `service`     |                       | string              | 服务名                   |
| ├            | `namespace`   |                       | string              | 命名空间                 |
| ├            | `revision`    |                       | string              | 版本信息                 |
| ├            | `metadata`    |                       | map<string, string> | 服务元数据               |
| ├            | `weight_type` |                       | string              | 权重类型                 |
| ├            | `subset`      |                       | map<string, string> | 实例所属的subset         |
| ⎿            | `instances`   |                       | map<string, object> | 服务实例列表             |
|              | ├             | `instance_id`         | string              | 服务实例ID               |
|              | ├             | `host`                | string              | 服务的节点IP或者域名     |
|              | ├             | `port`                | int                 | 节点端口号               |
|              | ├             | `container_name`      | string              | 实例元数据信息中的容器名 |
|              | ├             | `internal_set_name`   | string              | 实例元数据信息中的set名  |
|              | ├             | `logic_set`           | string              | 实例LogicSet信息         |
|              | ├             | `region`              | string              | location region          |
|              | ├             | `zone`                | string              | location zone            |
|              | ├             | `campus`              | string              | location campus          |
|              | ├             | `vpc_id`              | string              | 获取服务实例所在VPC ID   |
|              | ├             | `protocol`            | string              | 实例协议信息             |
|              | ├             | `version`             | string              | 实例版本号信息           |
|              | ├             | `weight`              | int                 | 实例静态权重值, 0-1000   |
|              | ├             | `priority`            | int                 | 实例优先级               |
|              | ├             | `dynamic_weight`      | int                 | 实例动态权重             |
|              | ├             | `hash_key`            | int                 | GetHash                  |
|              | ├             | `locality_aware_info` | int                 | locality_aware_info      |
|              | ├             | `healthy`             | bool                | 实例健康状态             |
|              | ├             | `isola`               | bool                | 实例隔离状态             |
|              | ⎿             | `metadata`            | map<string, string> | 实例元数据信息           |
> 具体代码使用示例

```php
$polaris_client = new PolarisClient(array());

// 初始化服务提供者的能力
$polaris_client->InitConsumer();

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

$timeout = 500;
$flow_id = 123456;

$res = $polaris_client->GetAllInstances($init_service_info, $timeout, $flow_id);
var_dump($res);
```

### UpdateServiceCallResult


> 方法用途

用于提前初始化服务数据

> 参数描述

*请求参数*

| 一级参数名称 | 二级参数名称          | 三级参数名称 | 参数类型            | 参数用途                                                                                        |
| :----------- | :-------------------- | :----------- | :------------------ | :---------------------------------------------------------------------------------------------- |
| `timeout`    |                       |              | long                | 超时时间，单位毫秒                                                                              |
| `flowId`     |                       |              | long                | 本次请求的标识ID                                                                                |
| `instance`   |                       |              | map<string, object> |                                                                                                 |
| ├            | `namespace`           |              | string              | 被调服务所在的命名空间                                                                          |
| ├            | `service`             |              | string              | 被调服务名称                                                                                    |
| ├            | `instance_id`         |              | string              | 服务实例ID                                                                                      |
| ├            | `host`                |              | string              | 服务实例Host(可选，如果设置了服务实例ID，则这个可不设置，优先使用服务实例ID)                    |
| ├            | `port`                |              | string              | 服务实例Port(可选，如果设置了服务实例ID，则这个可不设置，优先使用服务实例ID)，字符串数字："200" |
| ├            | `delay`               |              | string              | 设置服务实例调用时延，字符串数字："200"                                                         |
| ├            | `locality_aware_info` |              | string              | 设置需要传递的LocalityAware的信息，字符串数字："200"                                            |
| ├            | `ignore_half_open`    |              | string              | 设置是否略过跳过半开探测节点, "true" or "false"                                                 |
| ├            | `ret_status`          |              | string              | 调用返回状态码，字符串数字："200"                                                               |
| ├            | `ret_code`            |              | string              | 设置调用返回码。可选，用于支持根据返回码实现自己的插件                                          |
| ├            | `subset`              |              | map<string, string> | 设置被调服务subset信息                                                                          |
| ├            | `labels`              |              | map<string, string> | 设置被调服务labels信息                                                                          |
| ⎿            | `source`              |              | map<string, string> | 设置源服务信息                                                                                  |
|              | ├                     | `namespace`  | string              | 实例的标签信息                                                                                  |
|              | ├                     | `service`    | string              | 实例的标签信息                                                                                  |
|              | ⎿                     | `metadata`   | map<string, string> | 实例的标签信息                                                                                  |



*函数执行返回*

| 一级参数名称 | 参数类型 | 参数用途       |
| :----------- | :------- | :------------- |
| `code`       | int      | 方法执行状态码 |
| `err_msg`    | string   | 错误信息       |


> 具体代码使用示例

```php
$polaris_client = new PolarisClient(array());

// 初始化服务提供者的能力
$polaris_client->InitConsumer();

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

$timeout = 500;
$flow_id = 123456;

$res = $polaris_client->UpdateServiceCallResult($init_service_info, $timeout, $flow_id);
var_dump($res);
```


### GetRouteRuleKeys

> 方法用途

用于提前初始化服务数据

> 参数描述

*请求参数*

| 一级参数名称 | 二级参数名称 | 参数类型            | 参数用途               |
| :----------- | :----------- | :------------------ | :--------------------- |
| `timeout`    |              | long                | 超时时间，单位毫秒     |
| `flowId`     |              | long                | 本次请求的标识ID       |
| `service`    |              | map<string, object> |                        |
| ├            | `namespace`  | string              | 被调服务所在的命名空间 |
| ⎿            | `service`    | string              | 被调服务名称           |




*函数执行返回*

| 一级参数名称 | 参数类型 | 参数用途            |
| :----------- | :------- | :------------------ |
| `code`       | int      | 方法执行状态码      |
| `err_msg`    | string   | 错误信息            |
| `rule_keys`  | []string | 规则里配置的所有key |


> 具体代码使用示例

```php
$polaris_client = new PolarisClient(array());

// 初始化服务提供者的能力
$polaris_client->InitConsumer();

$service_info = array(
	"namespace" => "default",
	"service" => "php_ext_test",
);

$timeout = 500;
$flow_id = 123456;

$res = $polaris_client->UpdateServiceCallResult($service_info, $timeout, $flow_id);
var_dump($res);

/*
arra(
    "code" =>  ,
    "err_msg" => ,
    "rule_keys" => array(
        "",
        "",
        "",
        ""
    )
)
 */
```


***

## LimitAPI

### FetchRule


> 方法用途

预拉取服务配置的限流规则

> 参数描述

*请求参数*

| 一级参数名称 | 二级参数名称 | 参数类型            | 参数用途           |
| :----------- | :----------- | :------------------ | :----------------- |
| `timeout`    |              | long                | 超时时间，单位毫秒 |
| `service`    |              | map<string, object> |                    |
| ├            | `namespace`  | string              | 命名空间           |
| ⎿            | `service`    | string              | 服务名称           |




*函数执行返回*

| 一级参数名称          | 参数类型 | 参数用途           |
| :-------------------- | :------- | :----------------- |
| `code`                | int      | 方法执行状态码     |
| `err_msg`             | string   | 错误信息           |
| `limit_rule_json_str` | string   | json格式的限流规则 |


> 具体代码使用示例

```php
$polaris_client = new PolarisClient(array());

// 初始化服务提供者的能力
$polaris_client->InitLimit();

$service_info = array(
	"namespace" => "default",
	"service" => "php_ext_test",
);

$timeout = 500;

$res = $polaris_client->FetchRule($service_info, $timeout);
var_dump($res);

/*
arra(
    "code" =>  ,
    "err_msg" => ,
    "limit_rule_json_str" => ""
)
 */
```

### FetchRuleLabelKeys


> 方法用途

用于提前初始化服务数据

> 参数描述

*请求参数*

| 一级参数名称 | 二级参数名称 | 参数类型            | 参数用途               |
| :----------- | :----------- | :------------------ | :--------------------- |
| `timeout`    |              | long                | 超时时间，单位毫秒     |
| `service`    |              | map<string, object> |                        |
| ├            | `namespace`  | string              | 被调服务所在的命名空间 |
| ⎿            | `service`    | string              | 被调服务名称           |




*函数执行返回*

| 一级参数名称       | 参数类型 | 参数用途                       |
| :----------------- | :------- | :----------------------------- |
| `code`             | int      | 方法执行状态码                 |
| `err_msg`          | string   | 错误信息                       |
| `limit_label_keys` | []string | 限流规则里配置的label的所有key |


> 具体代码使用示例

```php
$polaris_client = new PolarisClient(array());

// 初始化服务提供者的能力
$polaris_client->InitLimit();

$service_info = array(
	"namespace" => "default",
	"service" => "php_ext_test",
);

$timeout = 500;

$res = $polaris_client->FetchRuleLabelKeys($service_info, $timeout);
var_dump($res);

/*
arra(
    "code" =>  ,
    "err_msg" => ,
    "limit_label_keys" => array(
        "",
        "",
        "",
        ""
    )
)
 */
```

### GetQuota


> 方法用途

获取配额

> 参数描述

*请求参数*

| 一级参数名称 | 参数类型            | 参数用途                                                  |
| :----------- | :------------------ | :-------------------------------------------------------- |
| `subset`     | map<string, string> | 设置请求的所属服务子集，可选                              |
| `labels`     | map<string, string> | 设置标签用于选择限流配置                                  |
| `amount`     | string              | 设置请求需要分配的配额数量，可选，默认为1, 字符串数字:100 |
| `namespace`  | string              | 被调服务所在的命名空间                                    |
| `service`    | string              | 被调服务名称                                              |


*函数执行返回*

| 一级参数名称   | 二级参数名称        | 参数类型            | 参数用途                                                   |
| :------------- | :------------------ | :------------------ | :--------------------------------------------------------- |
| `code`         |                     | int                 | 方法执行状态码                                             |
| `err_msg`      |                     | string              | 错误信息                                                   |
| `quota_result` |                     | map<string, object> | 规则里配置的所有key                                        |
|                | `wait_time`         | long                | 规则里配置的所有key                                        |
|                | `degrade`           | bool                | 规则里配置的所有key                                        |
|                | `duration`          | long                | 规则里配置的所有key                                        |
|                | `quota_left`        | long                | 规则里配置的所有key                                        |
|                | `quota_all`         | long                | 规则里配置的所有key                                        |
|                | `quota_result_code` | long                | 配额获取结果(0:配额正常，1:配额被限流，2:需求需要等待重试) |


> 具体代码使用示例

```php
$polaris_client = new PolarisClient(array());

// 初始化服务提供者的能力
$polaris_client->InitLimit();

$quota_request = array(
	"namespace" => "default",
	"service" => "php_ext_test",
    "amount" => "",
    "subset" => array(
        "{key}" => "{value}"
    ),
    "labels" => array(
        "{key}" => "{value}"
    )
);

$res = $polaris_client->GetQuota($quota_request);
var_dump($res);

/*
arra(
    "code" =>  ,
    "err_msg" => ,
    "quota_result" => array(
        "wait_time" => 10,
        "degrade" => true,
        "duration" => ,
        "quota_left" => 5,
        "quota_all" => 10,
        "quota_result_code" => 0
    )
)
 */
```

### UpdateCallResult

> 方法用途

更新请求配额调用结果

> 参数描述

*请求参数*

| 一级参数名称    | 参数类型            | 参数用途                         |
| :-------------- | :------------------ | :------------------------------- |
| `namespace`     | long                | 超时时间，单位毫秒               |
| `service`       | long                | 超时时间，单位毫秒               |
| `response_time` | string              | 请求响应时间，字符串数字："1000" |
| `respont_type`  | string              | "Limit"、"Failed"、"OK"          |
| `respont_code`  | long                | 本次请求的标识ID                 |
| `labels`        | map<string, string> |                                  |
| `subset`        | map<string, string> |                                  |


*函数执行返回*

| 一级参数名称 | 参数类型 | 参数用途       |
| :----------- | :------- | :------------- |
| `code`       | int      | 方法执行状态码 |
| `err_msg`    | string   | 错误信息       |


> 具体代码使用示例

```php
$polaris_client = new PolarisClient(array());

// 初始化服务提供者的能力
$polaris_client->InitConsumer();

$call_result = array(
	"namespace" => "default",
	"service" => "php_ext_test",
    "response_time" => "100",
    "response_code" => "100",
    "response_type" => "100",
    "labels" => array(),
    "subset" => array(),
);

$res = $polaris_client->UpdateCallResult($call_result);
var_dump($res);

/*
arra(
    "code" =>  ,
    "err_msg" => ,
    "rule_keys" => array(
        "",
        "",
        "",
        ""
    )
)
 */
```

### InitQuotaWindow


> 方法用途

初始化配额窗口，可选调用，用于提前初始化配窗口减小首次配额延迟

> 参数描述

*请求参数*

| 一级参数名称 | 参数类型            | 参数用途                                              |
| :----------- | :------------------ | :---------------------------------------------------- |
| `subset`     | map<string, string> | 设置请求的所属服务子集，可选                          |
| `labels`     | map<string, string> | 设置标签用于选择限流配置                              |
| `amount`     | string              | 设置请求需要分配的配额数量，可选，默认为1，字符串数字 |
| `namespace`  | string              | 被调服务所在的命名空间                                |
| `service`    | string              | 被调服务名称                                          |


*函数执行返回*

| 一级参数名称 | 参数类型 | 参数用途       |
| :----------- | :------- | :------------- |
| `code`       | int      | 方法执行状态码 |
| `err_msg`    | string   | 错误信息       |


> 具体代码使用示例

```php
$polaris_client = new PolarisClient(array());

// 初始化服务提供者的能力
$polaris_client->InitLimit();

$quota_request = array(
	"namespace" => "default",
	"service" => "php_ext_test",
    "amount" => "",
    "subset" => array(
        "{key}" => "{value}"
    ),
    "labels" => array(
        "{key}" => "{value}"
    )
);

$res = $polaris_client->InitQuotaWindow($quota_request);
var_dump($res);

/*
arra(
    "code" =>  ,
    "err_msg" => ,
    "rule_keys" => array(
        "",
        "",
        "",
        ""
    )
)
 */
```
