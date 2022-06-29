<?php
/**
* Tencent is pleased to support the open source community by making Polaris available.
*
* Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
*
* Licensed under the BSD 3-Clause License (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* https://opensource.org/licenses/BSD-3-Clause
*
* Unless required by applicable law or agreed to in writing, software distributed
* under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
* CONDITIONS OF ANY KIND, either express or implied. See the License for the
* specific language governing permissions and limitations under the License.
*/


/**
 * Tencent polaris 北极星php-sdk
 */
class Polaris
{
    //load_balance_type  Enum
    const Balance_WeightedRandom = "weightedRandom";    //权重随机
    const Balance_RingHash = "ringHash";                //一致性hash负载均衡
    const Balance_Maglev = "maglev";                    //一致性Hash: maglev 算法
    const Balance_L5cst = "l5cst";                      //兼容L5的一致性Hash
    const Balance_SimpleHash = "simpleHash";            //hash_key%总实例数 选择服务实例
    const Balance_CMurmurHash = "cMurmurHash";          //兼容brpc c_murmur的一致性哈希
    const Balance_LocalityAware = "localityAware";      //兼容brpc locality_aware的负载均衡
    const Balance_Defaults = "default";                 //使用全局配置的负载均衡算法

    //metadata_failover_type  Enum
    const Failover_NotKey = "notKey";                     //返回不包含元数据路由key的节点
    const Failover_All = "all";                          //降级返回所有节点
    const Failover_None = "none";                        //默认不降级

    //weight_type       Enum
    const Weight_Static = "static";                     //静态权重
    const Weight_Dynamic = "dynamic";                   //动态权重

    //ret_status        Enum
    const Ret_Status_OK = "ok";                         //服务实例正常
    const Ret_Status_Error = "error";                   //服务实例错误
    const Ret_Status_Timeout = "timeout";               //服务实例超时

    //respont_type
    const Respont_Type_OK = "OK";
    const Respont_Type_Failed = "Failed";
    const Respont_Type_Limit = "Limit";

    private $default_config = array("config_path" => "./polaris.yaml", "log_dir" => "./");
    protected $_polarisClient;

    /**
     * 构造方法
     * @param string $configPath  yaml配置文件地址
     * @param string $logDir       日志地址
     * @return void
     */
    public function __construct($configPath, $logDir)
    {
        if (empty($configPath)) {
            $configPath = $this->default_config["config_path"];
        }
        if (empty($logDir)) {
            $logDir = $this->default_config["log_dir"];
        }
        $this->_polarisClient = new PolarisClient(array("config_path" => $configPath, "log_dir" => $logDir));
        $this->_polarisClient->InitProvider();
        $this->_polarisClient->InitConsumer();
        $this->_polarisClient->InitLimit();
    }

    /**
     * 用于开启 PolarisClient的 ProviderAPI的相关能力
     * 注册服务实例
     * 反注册实例子
     * 上报服务实例心跳
     * @return void
     */
    protected function InitProvider()
    {
        $this->_polarisClient->InitProvider();
    }

    /**
     * 用于开启 PolarisClient的 ConsumerAPI的相关能力
     * 获取单个服务的一个实例
     * 获取单个服务的实例
     * 上报服务调用信息
     * @return void
     */
    protected function InitConsumer()
    {
        $this->_polarisClient->InitConsumer();
    }

    /**
     * 用于开启 PolarisClient的 LimitAPI的相关能力
     * 获取限流规则的配额信息
     * 查看当前限流结果
     * 更新请求配额调用结果
     * @return void
     */
    protected function InitLimit()
    {
        $this->_polarisClient->InitLimit();
    }

    /*  ProviderAPI  */
    /**
     * 注册服务
     * @param int $timeout 超时时间，单位毫秒
     * @param int $flowId 本次请求的标识ID
     * @param array $instanceInfo = array(
     *      "namespace" => "",  //服务所在的命名空间
     *      "service" => "",  //服务名称
     *      "host" => "",  //实例的IP
     *      "port" => "",  //实例的端口，字符串数字: "100"
     *      "protocol" => "",  //实例端口的协议
     *      "priority" => "",  //实例优先级，字符串数字: "100"
     *      "weight" => "",  //实例的权重，字符串数字: "100"
     *      "version" => "",  //实例版本
     *      "vpc_id" => "",  //实例所在的VPC_ID信息
     *      "heartbeat" => "",  //是否开启心服务端对本实例执行跳健康检查，eg: "true"
     *      "ttl" => "",  //心跳间隔时间，单位为秒，仅在 heartbeat 为 true 下生效，eg: "100"
     *      "metadata" => array(),  //实例的标签信息
     * )
     *
     * @return array $res = array(
     *      "code" => 0,   //方法执行状态码
     *      "err_msg" => "",  //错误信息
     *      "instance_id" => "",  //注册成功，实例的唯一ID标识
     * )
     */
    public function Register($instanceInfo, $timeout, $flowId)
    {
        return $this->_polarisClient->Register($instanceInfo, $timeout, $flowId);
    }

    /**
     * 提供实例反注册功能，即主动将该实例从北极星中彻底删除
     * @param int $timeout 超时时间，单位毫秒
     * @param int $flowId 本次请求的标识ID
     * @param array $instanceInfo = array(
     *      "namespace" => "",  //服务所在的命名空间
     *      "service" => "",  //服务名称
     *      "host" => "",  //实例的IP
     *      "port" => "",  //实例的端口，字符串数字: "100"
     *      "vpc_id" => "",  //实例所在的VPC_ID信息
     * )
     *
     * @return array $res = array(
     *      "code" => 0,   //方法执行状态码
     *      "err_msg" => "",  //错误信息
     * )
     */
    public function Deregister($instanceInfo, $timeout, $flowId)
    {
        return $this->_polarisClient->Deregister($instanceInfo, $timeout, $flowId);
    }

    /**
     * 提供实例反注册功能，即主动将该实例从北极星中彻底删除
     * @param string $namespace 服务所在的命名空间
     * @param string $service 服务名称
     * @param string $host 实例的IP
     * @param string $port 实例的端口，字符串数字: "100"
     * @param string $vpcId 实例所在的VPC_ID信息
     * @return array
     */
    public function Heartbeat($namespace, $service, $host, $port, $vpcId)
    {
        return $this->_polarisClient->Heartbeat(array(
            "namespace" => $namespace,
            "service" => $service,
            "host" => $host,
            "port" => $port,
            "vpc_id" => $vpcId,
        ));
    }

    /*  ConsmerAPI  */
    /**
     * 用于提前初始化服务数据
     * @param int $timeout 超时时间，单位毫秒
     * @param int $flowId 本次请求的标识ID
     * @param array $instanceInfo = array(
     *      "namespace" => "",  //服务所在的命名空间
     *      "service" => "",  //服务名称
     *      "version" => "",  //实例版本
     *      "vpc_id" => "",  //实例所在的VPC_ID信息
     *      "metadata" => array(), //设置元数据，用于元数据路由
     *      "canary" => "", //设置调用哪个金丝雀服务实例
     *      "source_set_name" => "", //设置调用哪个set下的服务
     *      "ignore_half_open" => "true",  //设置是否略过跳过半开探测节点，eg:"true"
     *      "hash_string" => "",  //设置 hash 字符串，用于一致性哈希负载均衡算法
     *      "hash_key" => "",  //设置hash key，用于一致性哈希负载均衡算法，eg:"123"
     *      "replicate_index" => "",  //用于一致性hash算法时获取副本实例，eg:"123"
     *      "backup_instance_num" => "", //设置用于重试的实例数。可选，默认不返回用于重试的实例，eg:"123"
     *      "load_balance_type" => Polaris::Balance_WeightedRandom, //设置负载均衡类型。可选，默认使用配置文件中设置的类型
     *      "metadata_failover_type" => Polaris::Failover_None, //设置元数据路由匹配失败时的降级策略，默认不降级
     *      "labels" = array(), //设置请求标签，用于接口级别熔断
     *      "source" => array(      //设置源服务信息，用于服务路由计算。可选
     *          "namespace" => "",  //实例的标签信息
     *          "service" => "",    //实例的标签信息
     *          "metadata" => "",   //实例的标签信息
     *       )
     * )
     *
     * @return array $res = array(
     *      "code" => 0,   //方法执行状态码
     *      "err_msg" => "",  //错误信息
     * )
     */
    public function InitService($instanceInfo, $timeout, $flowId)
    {
        return $this->_polarisClient->InitService($instanceInfo, $timeout, $flowId);
    }


    /**
     * 用于提前初始化服务数据
     * @param int $timeout 超时时间，单位毫秒
     * @param int $flowId 本次请求的标识ID
     * @param array $instanceInfo = array(
     *      "namespace" => "",  //服务所在的命名空间
     *      "service" => "",  //服务名称
     *      "version" => "",  //实例版本
     *      "vpc_id" => "",  //实例所在的VPC_ID信息
     *      "metadata" => array(), //设置元数据，用于元数据路由
     *      "canary" => "", //设置调用哪个金丝雀服务实例
     *      "source_set_name" => "", //设置调用哪个set下的服务
     *      "ignore_half_open" => "true",  //设置是否略过跳过半开探测节点，eg:"true"
     *      "hash_string" => "",  //设置 hash 字符串，用于一致性哈希负载均衡算法
     *      "hash_key" => "",  //设置hash key，用于一致性哈希负载均衡算法，eg:"123"
     *      "replicate_index" => "",  //用于一致性hash算法时获取副本实例，eg:"123"
     *      "backup_instance_num" => "", //设置用于重试的实例数。可选，默认不返回用于重试的实例，eg:"123"
     *      "load_balance_type" => Polaris::Balance_WeightedRandom, //设置负载均衡类型。可选，默认使用配置文件中设置的类型
     *      "metadata_failover_type" => Polaris::Failover_None, //设置元数据路由匹配失败时的降级策略，默认不降级
     *      "labels" = array(), //设置请求标签，用于接口级别熔断
     *      "source" => array(      //设置源服务信息，用于服务路由计算。可选
     *          "namespace" => "",  //实例的标签信息
     *          "service" => "",    //实例的标签信息
     *          "metadata" => array(),   //实例的标签信息
     *       )
     * )
     *
     * @return array $res = array(
     *      "code" => 0,   //方法执行状态码
     *      "err_msg" => "",  //错误信息
     *      "response" => array(
     *          "flow_id" => 0,  //请求流水号
     *          "service" => "",  //服务名
     *          "namespace" => "",  //命名空间
     *          "revision" => "",  //版本信息
     *          "metadata" => array(),  //服务元数据
     *          "weight_type" => Polaris::Weight_Static,  //权重类型
     *          "subset" => array(),  //实例所属的subset
     *          "instances" => array(    //服务实例列表
     *              "instance_id" => "",  //服务实例ID
     *              "host" => "",  //服务的节点IP或者域名
     *              "port" => 32886,  //节点端口号
     *              "container_name" => "",  //实例元数据信息中的容器名
     *              "internal_set_name" => "",  //实例元数据信息中的set名
     *              "logic_set" => "",  //实例LogicSet信息
     *              "region" => "",  //location region
     *              "zone" => "",  //location zone
     *              "campus" => "",  //    location campus
     *              "vpc_id" => "",  //获取服务实例所在VPC ID
     *              "protocol" => "",  //实例协议信息
     *              "version" => 1,  //实例版本号信息
     *              "weight" => 100,  //实例静态权重值, 0-1000
     *              "priority" => 888,  //实例优先级
     *              "dynamic_weight" => 123,  //实例动态权重
     *              "hash_key" => 0,  //GetHash
     *              "locality_aware_info" => 0,  //locality_aware_info
     *              "healthy" => true,  //实例健康状态
     *              "isola" => false, //实例隔离状态
     *              "metadata" => array(), //实例元数据信息
     *          )
     *      )
     * )
     */
    public function GetOneInstance($instanceInfo, $timeout, $flowId)
    {
        return $this->_polarisClient->GetOneInstance($instanceInfo, $timeout, $flowId);
    }

    /**
     * 用于提前初始化服务数据
     * @param int $timeout 超时时间，单位毫秒
     * @param int $flowId 本次请求的标识ID
     * @param array $instanceInfo = array(
     *      "namespace" => "",  //服务所在的命名空间
     *      "service" => "",  //服务名称
     *      "include_unhealthy_instances" => "true", //设置服务路由时否包含不健康的服务实例。可选，默认不包含, 字符串bool值："true"
     *      "include_circuit_breaker_instances" => "true", //设置服务路由时是否包含熔断的服务实例。可选，默认不包含, 字符串bool值："true"
     *      "skip_route_filter" => "true", //设置是否跳过服务路由。可选，默认不跳过服务路由, 字符串bool值："true"
     *      "canary" => "", //设置调用哪个金丝雀服务实例
     *      "source_set_name" => "", //设置调用哪个set下的服务
     *      "metadata_failover_type" => Polaris::Failover_None, //设置元数据路由匹配失败时的降级策略，默认不降级
     *      "source" => array(      //设置源服务信息，用于服务路由计算。可选
     *          "namespace" => "",  //实例的标签信息
     *          "service" => "",    //实例的标签信息
     *          "metadata" => array(),   //实例的标签信息
     *       )
     * )
     *
     * @return array $res = array(
     *      "code" => 0,   //方法执行状态码
     *      "err_msg" => "",  //错误信息
     *      "response" => array(
     *          "flow_id" => 0,  //请求流水号
     *          "service" => "",  //服务名
     *          "namespace" => "",  //命名空间
     *          "revision" => "",  //版本信息
     *          "metadata" => array(),  //服务元数据
     *          "weight_type" => Polaris::Weight_Static,  //权重类型 字符串
     *          "subset" => array(),  //实例所属的subset
     *          "instances" => array(    //服务实例列表
     *              "instance_id" => "",  //服务实例ID
     *              "host" => "",  //服务的节点IP或者域名
     *              "port" => 32886,  //节点端口号
     *              "container_name" => "",  //实例元数据信息中的容器名
     *              "internal_set_name" => "",  //实例元数据信息中的set名
     *              "logic_set" => "",  //实例LogicSet信息
     *              "region" => "",  //location region
     *              "zone" => "",  //location zone
     *              "campus" => "",  //    location campus
     *              "vpc_id" => "",  //获取服务实例所在VPC ID
     *              "protocol" => "",  //实例协议信息
     *              "version" => 1,  //实例版本号信息
     *              "weight" => 100,  //实例静态权重值, 0-1000
     *              "priority" => 888,  //实例优先级
     *              "dynamic_weight" => 123,  //实例动态权重
     *              "hash_key" => 0,  //GetHash
     *              "locality_aware_info" => 0,  //locality_aware_info
     *              "healthy" => true,  //实例健康状态
     *              "isola" => false, //实例隔离状态
     *              "metadata" => array(), //实例元数据信息
     *          )
     *      )
     * )
     */
    public function GetInstances($instanceInfo, $timeout, $flowId)
    {
        return $this->_polarisClient->GetInstances($instanceInfo, $timeout, $flowId);
    }

    /**
     * 用于提前初始化服务数据
     * @param int $timeout 超时时间，单位毫秒
     * @param int $flowId 本次请求的标识ID
     * @param array $instanceInfo = array(
     *      "namespace" => "",  //服务所在的命名空间
     *      "service" => "",  //服务名称
     *      "include_unhealthy_instances" => "true", //设置服务路由时否包含不健康的服务实例。可选，默认不包含, 字符串bool值："true"
     *      "include_circuit_breaker_instances" => "true", //设置服务路由时是否包含熔断的服务实例。可选，默认不包含, 字符串bool值："true"
     *      "skip_route_filter" => "true", //设置是否跳过服务路由。可选，默认不跳过服务路由, 字符串bool值："true"
     *      "canary" => "", //设置调用哪个金丝雀服务实例
     *      "source_set_name" => "", //设置调用哪个set下的服务
     *      "metadata_failover_type" => Polaris::Failover_None, //设置元数据路由匹配失败时的降级策略，默认不降级
     *      "source" => array(      //设置源服务信息，用于服务路由计算。可选
     *          "namespace" => "",  //实例的标签信息
     *          "service" => "",    //实例的标签信息
     *          "metadata" => array(),   //实例的标签信息
     *       )
     * )
     *
     * @return array $res = array(
     *      "code" => 0,   //方法执行状态码
     *      "err_msg" => "",  //错误信息
     *      "response" => array(
     *          "flow_id" => 0,  //请求流水号
     *          "service" => "",  //服务名
     *          "namespace" => "",  //命名空间
     *          "revision" => "",  //版本信息
     *          "metadata" => array(),  //服务元数据
     *          "weight_type" => Polaris::Weight_Static,  //权重类型 字符串
     *          "subset" => array(),  //实例所属的subset
     *          "instances" => array(    //服务实例列表
     *              "instance_id" => "",  //服务实例ID
     *              "host" => "",  //服务的节点IP或者域名
     *              "port" => 32886,  //节点端口号
     *              "container_name" => "",  //实例元数据信息中的容器名
     *              "internal_set_name" => "",  //实例元数据信息中的set名
     *              "logic_set" => "",  //实例LogicSet信息
     *              "region" => "",  //location region
     *              "zone" => "",  //location zone
     *              "campus" => "",  //    location campus
     *              "vpc_id" => "",  //获取服务实例所在VPC ID
     *              "protocol" => "",  //实例协议信息
     *              "version" => 1,  //实例版本号信息
     *              "weight" => 100,  //实例静态权重值, 0-1000
     *              "priority" => 888,  //实例优先级
     *              "dynamic_weight" => 123,  //实例动态权重
     *              "hash_key" => 0,  //GetHash
     *              "locality_aware_info" => 0,  //locality_aware_info
     *              "healthy" => true,  //实例健康状态
     *              "isola" => false, //实例隔离状态
     *              "metadata" => array(), //实例元数据信息
     *          )
     *      )
     * )
     */
    public function GetAllInstances($instanceInfo, $timeout, $flowId)
    {
        return $this->_polarisClient->GetAllInstances($instanceInfo, $timeout, $flowId);
    }


    /**
     * 用于提前初始化服务数据
     * @param int $timeout 超时时间，单位毫秒
     * @param int $flowId 本次请求的标识ID
     * @param array $instanceInfo = array(
     *      "namespace" => "",  //服务所在的命名空间
     *      "service" => "",  //服务名称
     *      "instance_id" => "", //服务实例ID
     *      "host" => "true", //服务实例Host(可选，如果设置了服务实例ID，则这个可不设置，优先使用服务实例ID)
     *      "port" => "200", //服务实例Port(可选，如果设置了服务实例ID，则这个可不设置，优先使用服务实例ID)，字符串数字："200"
     *      "delay" => "200", //设置服务实例调用时延，字符串数字："200"
     *      "locality_aware_info" => "200", //设置需要传递的LocalityAware的信息，字符串数字："200"
     *      "ret_status" => Polaris::Ret_Status_OK, //调用返回状态 枚举字符串
     *      "ret_code" => "", //设置调用返回码。可选，用于支持根据返回码实现自己的插件
     *      "subset" => array(), //设置被调服务subset信息
     *      "labels" => array(), //设置被调服务labels信息
     * )
     *
     * @return array $res = array(
     *      "code" => 0,   //方法执行状态码
     *      "err_msg" => "",  //错误信息
     * )
     */
    public function UpdateServiceCallResult($instanceInfo, $timeout, $flowId)
    {
        return $this->_polarisClient->UpdateServiceCallResult($instanceInfo, $timeout, $flowId);
    }

    /*  LimitAPI  */
    /**
     * 获取配额
     * @param string $namespace 被调服务所在的命名空间
     * @param string $service 被调服务所在的服务名称
     * @param string $amount 设置请求需要分配的配额数量，可选，默认为1, 字符串数字:100
     * @param array $labels 设置标签用于选择限流配置
     * @param array $subset 设置请求的所属服务子集，可选
     *
     * @return array $res = array(
     *      "code" => 0,   //方法执行状态码
     *      "err_msg" => "",  //错误信息
     *      "quota_result" => array(   //规则里配置的所有key
     *          "wait_time" => 12345, //请求需要获取多长时间才能使用配额
     *          "degrade" => false, //是否降级
     *          "duration" => 3600, //配置周期
     *          "quota_left" => 22, //剩余配额
     *          "quota_all" => 11, //配置的配额
     *          "quota_result_code" => 0, //配额获取结果(0:配额正常，1:配额被限流，2:需求需要等待重试)
     *      )
     * )
     */
    public function GetQuota($namespace, $service, $amount, $labels, $subset = array())
    {
        return $this->_polarisClient->GetQuota(array(
            "namespace" => $namespace,
            "service" => $service,
            "amount" => (string)$amount,
            "labels" => $labels,
            "subset" => $subset,
        ));
    }

    /**
     * 更新请求配额调用结果
     * @param string $namespace 命名空间
     * @param string $service 服务名称
     * @param string $response_time 请求响应时间，字符串数字："1000"
     * @param string $respont_type Polaris::Respont_Type_Limit | Polaris::Respont_Type_Failed | Polaris::Respont_Type_OK
     * @param int $respont_code 本次请求的标识ID
     * @param array $labels 设置标签用于选择限流配置
     * @param array $subset 设置请求的所属服务子集，可选
     *
     * @return array $res = array(
     *      "code" => 0,   //方法执行状态码
     *      "err_msg" => "",  //错误信息
     * )
     */
    public function UpdateCallResult($namespace, $service, $response_time, $respont_type, $respont_code, $labels, $subset = array())
    {
        return $this->_polarisClient->UpdateCallResult(array(
            "namespace" => $namespace,
            "service" => $service,
            "response_time" => (string)$response_time,
            "respont_code" => $respont_code,
            "respont_type" => $respont_type,
            "labels" => $labels,
            "subset" => $subset,
        ));
    }

    /**
     * 初始化配额窗口，可选调用，用于提前初始化配窗口减小首次配额延迟
     * @param string $namespace 被调服务所在的命名空间
     * @param string $service 被调服务所在的服务名称
     * @param string $amount 设置请求需要分配的配额数量，可选，默认为1, 字符串数字:100
     * @param array $labels 设置标签用于选择限流配置
     * @param array $subset 设置请求的所属服务子集，可选
     *
     * @return array $res = array(
     *      "code" => 0,   //方法执行状态码
     *      "err_msg" => "",  //错误信息
     * )
     */
    public function InitQuotaWindow($namespace, $service, $amount, $labels, $subset = array())
    {
        return $this->_polarisClient->InitQuotaWindow(array(
            "namespace" => $namespace,
            "service" => $service,
            "amount" => (string)$amount,
            "labels" => $labels,
            "subset" => $subset,
        ));
    }
}