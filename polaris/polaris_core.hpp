using namespace std;

extern "C"
{
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_polaris.h"
}

#include <string>
#include <iostream>
#include <vector>

// 引入 Polaris ProviderApi 函数定义
#include "polaris/provider.h"
#include "polaris/consumer.h"
#include "polaris/config.h"
#include "polaris/context.h"
#include "polaris/log.h"

#include "provider_core.hpp"
#include "consumer_core.hpp"
#include "limit_core.hpp"
#include "common.hpp"
#include "utils.hpp"

// 定义一个 PHP 对象用于持有 ProviderApi 这个对象的实例
zend_object_handlers polaris_core_object_handlers;

zend_function_entry polaris_core_functions[] = {
    PHP_ME(PolarisClient, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR) /**/
    PHP_ME(PolarisClient, InitProvider, NULL, ZEND_ACC_PUBLIC)                /**/
    PHP_ME(PolarisClient, InitConsumer, NULL, ZEND_ACC_PUBLIC)                /**/
    PHP_ME(PolarisClient, InitLimit, NULL, ZEND_ACC_PUBLIC)                   /**/

    // Provider
    PHP_ME(PolarisClient, Register, NULL, ZEND_ACC_PUBLIC)   /**/
    PHP_ME(PolarisClient, Deregister, NULL, ZEND_ACC_PUBLIC) /**/
    PHP_ME(PolarisClient, Heartbeat, NULL, ZEND_ACC_PUBLIC)  /**/

    // Consumer
    PHP_ME(PolarisClient, InitService, NULL, ZEND_ACC_PUBLIC)             /**/
    PHP_ME(PolarisClient, GetOneInstance, NULL, ZEND_ACC_PUBLIC)          /**/
    PHP_ME(PolarisClient, GetInstances, NULL, ZEND_ACC_PUBLIC)            /**/
    PHP_ME(PolarisClient, GetAllInstances, NULL, ZEND_ACC_PUBLIC)         /**/
    PHP_ME(PolarisClient, UpdateServiceCallResult, NULL, ZEND_ACC_PUBLIC) /**/
    PHP_ME(PolarisClient, GetRouteRuleKeys, NULL, ZEND_ACC_PUBLIC)        /**/

    // Limit
    PHP_ME(PolarisClient, FetchRule, NULL, ZEND_ACC_PUBLIC)          /**/
    PHP_ME(PolarisClient, FetchRuleLabelKeys, NULL, ZEND_ACC_PUBLIC) /**/
    PHP_ME(PolarisClient, GetQuota, NULL, ZEND_ACC_PUBLIC)           /**/
    PHP_ME(PolarisClient, UpdateCallResult, NULL, ZEND_ACC_PUBLIC)   /**/
    PHP_ME(PolarisClient, InitQuotaWindow, NULL, ZEND_ACC_PUBLIC)    /**/
    PHP_FE_END};

struct polaris_core_object
{
    zend_object std;
    polaris::Context *globalCtx;
    polaris::ProviderApi *provider;
    polaris::ConsumerApi *consumer;
    polaris::LimitApi *limit;
};

void polaris_core_free_storage(void *object TSRMLS_DC)
{
    polaris_core_object *obj = (polaris_core_object *)object;
    delete obj->provider;
    delete obj->consumer;
    delete obj->limit;

    zend_object_std_dtor(&obj->std TSRMLS_CC);
    efree(obj);
}

zend_object_value polaris_core_create_handler(zend_class_entry *cls_type TSRMLS_DC)
{
    zend_object_value retval;
    polaris_core_object *intern;

    /* Allocate memory for it */
    intern = (polaris_core_object *)emalloc(sizeof(polaris_core_object));
    memset(intern, 0, sizeof(polaris_core_object));

    zend_object_std_init(&intern->std, cls_type TSRMLS_CC);
    object_properties_init(&intern->std, cls_type);

    retval.handle = zend_objects_store_put(
        intern,
        (zend_objects_store_dtor_t)zend_objects_destroy_object,
        (zend_objects_free_object_storage_t)polaris_core_free_storage,
        NULL TSRMLS_CC);
    retval.handlers = (zend_object_handlers *)&polaris_core_object_handlers;

    return retval;
}

// 定义 PolarisClient 的构造函数，这里就只支持一种吧，直接传入对应的配置文件地址用于初始化
PHP_METHOD(PolarisClient, __construct)
{
    zval *arg;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &arg))
    {
        zend_error(E_WARNING, "get parameter for config path failed");
        RETURN_LONG(-1);
    }

    HashTable *paramMap = Z_ARRVAL_P(arg);

    map<string, string> params = TransferToStdMap(paramMap);

    // 设置日志级别
    polaris::GetLogger()->SetLogLevel(polaris::kTraceLogLevel);
    polaris::GetLogger()->SetLogDir(params[LogDir]);

    string errMsg;

    polaris::Config *config = polaris::Config::CreateFromFile(params[ConfigPath], errMsg);

    if (config == nullptr || config == NULL)
    {
        zend_error(E_WARNING, "create polaris config failed");
        RETURN_NULL();
    }

    polaris_core_object *obj = (polaris_core_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

    if (obj == nullptr)
    {
        RETURN_NULL();
    }

    // static PHP_METHOD(PDO, dbh_constructor)
    obj->globalCtx = polaris::Context::Create(config, polaris::kShareContext);
    obj->provider = NULL;
    obj->consumer = NULL;
    obj->limit = NULL;
}

//
PHP_METHOD(PolarisClient, InitProvider)
{
    polaris_core_object *obj = (polaris_core_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

    if (obj == nullptr)
    {
        RETURN_NULL();
    }
    if (obj->provider == nullptr)
    {
        obj->provider = polaris::ProviderApi::Create(obj->globalCtx);
    }
}

// 注册一个服务实例
PHP_METHOD(PolarisClient, Register)
{
    // 注册实例信息原始参数信息
    zval *registerVal, *metadataVal;
    uint64_t timeout = 500;
    uint64_t flowId;

    string errMsg = "";

    if (ZEND_NUM_ARGS() == 4)
    {
        // 识别参数信息
        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "aa|ll", &registerVal, &metadataVal, &timeout, &flowId))
        {
            zend_error(E_WARNING, "Wrong parameter parse parameters for Register()");
            RETURN_LONG(-1);
        }
    }
    else
    {
        WRONG_PARAM_COUNT;
        RETURN_LONG(-1);
    }

    // 找到对应的 C++ 实例缓存信息
    polaris_core_object *obj = (polaris_core_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

    polaris::ProviderApi *provider = obj->provider;
    if (provider != nullptr)
    {
        polaris::ReturnCode code = RegisterInstance(provider, registerVal, metadataVal, timeout, flowId);
        errMsg = polaris::ReturnCodeToMsg(code);
        add_assoc_stringl(registerVal, ErrMsg, (char *)errMsg.c_str(), errMsg.length(), 1);
        RETURN_LONG(code);
    }
    errMsg = "polaris provider instance is nullptr";
    add_assoc_stringl(registerVal, ErrMsg, (char *)errMsg.c_str(), errMsg.length(), 1);
    RETURN_LONG(-1)
}

// 反注册实例
PHP_METHOD(PolarisClient, Deregister)
{
    // 注册实例信息原始参数信息
    zval *deregisterVal;
    uint64_t timeout = 500;
    uint64_t flowId;

    std::string errMsg = "";

    if (ZEND_NUM_ARGS() == 3)
    {
        // 识别参数信息
        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|ll", &deregisterVal, &timeout, &flowId))
        {
            RETURN_LONG(-1);
        }
    }
    else
    {
        WRONG_PARAM_COUNT;
        RETURN_LONG(-1);
    }

    // 将参数转换为 map<string, string> 对象
    map<string, string> params = TransferToStdMap(Z_ARRVAL_P(deregisterVal));

    int port = atoi(params[Port].c_str());
    string tmp = params[Token];
    string token = tmp.empty() ? "polaris_php_sdk__" + params[Service] : tmp;

    // 进行 InstanceDeregisterRequest 请求参数的初始化动作
    polaris::InstanceDeregisterRequest req(params[Namespace], params[Service], token, params[Host], port);
    req.SetTimeout(timeout);
    req.SetFlowId(flowId);
    req.SetVpcId(params[VpcID]);

    // 找到对应的 C++ 实例缓存信息
    polaris::ProviderApi *provider;
    polaris_core_object *obj = (polaris_core_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    provider = obj->provider;
    if (provider != nullptr)
    {
        polaris::ReturnCode code = provider->Deregister(req);
        // 返回值回插入到 register_req 中？
        errMsg = polaris::ReturnCodeToMsg(code);
        add_assoc_stringl(deregisterVal, ErrMsg, (char *)errMsg.c_str(), errMsg.length(), 1);
        RETURN_LONG(code);
    }
    errMsg = "polaris provider instance is nullptr";
    add_assoc_stringl(deregisterVal, ErrMsg, (char *)errMsg.c_str(), errMsg.length(), 1);
    RETURN_LONG(-1)
}

// 执行心跳
PHP_METHOD(PolarisClient, Heartbeat)
{
    // 实例心跳信息原始参数信息
    zval *heartbeatVal;
    uint64_t timeout = 500;
    uint64_t flowId;

    std::string errMsg = "";

    if (ZEND_NUM_ARGS() == 3)
    {
        // 识别参数信息
        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "all", &heartbeatVal, &timeout, &flowId))
        {
            RETURN_LONG(-1);
        }
    }
    else
    {
        WRONG_PARAM_COUNT;
        RETURN_LONG(-1);
    }

    map<string, string> params = TransferToStdMap(Z_ARRVAL_P(heartbeatVal));

    int port = atoi(params[Port].c_str());
    string tmp = params[Token];
    string token = tmp.empty() ? "polaris_php_sdk__" + params[Service] : tmp;

    polaris::InstanceHeartbeatRequest req(params[Namespace], params[Service], token, params[Host], port);
    req.SetTimeout(timeout);
    req.SetFlowId(flowId);
    req.SetVpcId(params[VpcID]);

    // 找到对应的 C++ 实例缓存信息
    polaris::ProviderApi *provider;
    polaris_core_object *obj = (polaris_core_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    provider = obj->provider;
    if (provider != nullptr)
    {
        polaris::ReturnCode code = provider->Heartbeat(req);
        // 返回值回插入到 register_req 中？
        errMsg = polaris::ReturnCodeToMsg(code);
        add_assoc_stringl(heartbeatVal, ErrMsg, (char *)errMsg.c_str(), errMsg.length(), 1);
        RETURN_LONG(code);
    }
    errMsg = "polaris provider instance is nullptr";
    add_assoc_stringl(heartbeatVal, ErrMsg, (char *)errMsg.c_str(), errMsg.length(), 1);
    RETURN_LONG(-1)
}

/**/

//

PHP_METHOD(PolarisClient, FetchRule)
{
    RETURN_LONG(-1);
}

PHP_METHOD(PolarisClient, FetchRuleLabelKeys)
{

    RETURN_LONG(-1);
}

PHP_METHOD(PolarisClient, GetQuota)
{

    RETURN_LONG(-1);
}

PHP_METHOD(PolarisClient, UpdateCallResult)
{
    RETURN_LONG(-1);
}

PHP_METHOD(PolarisClient, InitQuotaWindow)
{
    RETURN_LONG(-1);
}

/**/
PHP_METHOD(PolarisClient, InitConsumer)
{
    polaris_core_object *obj = (polaris_core_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

    if (obj == nullptr)
    {
        RETURN_NULL();
    }
    obj->provider = polaris::ProviderApi::Create(obj->globalCtx);
}
//
PHP_METHOD(PolarisClient, InitLimit)
{
    polaris_core_object *obj = (polaris_core_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

    if (obj == nullptr)
    {
        RETURN_NULL();
    }

    obj->limit = polaris::LimitApi::Create(obj->globalCtx);
}

// 服务消费端API主接口
PHP_METHOD(PolarisClient, InitService)
{

    // 找到对应的 C++ 实例缓存信息
    polaris_core_object *obj = (polaris_core_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

    polaris::ConsumerApi *consumer = obj->consumer;
    if (consumer != nullptr)
    {
    }
    RETURN_LONG(-1);
}

// 服务消费端API主接口
// ReturnCode GetOneInstance(const GetOneInstanceRequest &req, Instance &instance);
PHP_METHOD(PolarisClient, GetOneInstance)
{

    // 找到对应的 C++ 实例缓存信息
    polaris_core_object *obj = (polaris_core_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

    polaris::ConsumerApi *consumer = obj->consumer;
    if (consumer != nullptr)
    {
    }
    RETURN_LONG(-1)
}

// 服务消费端API主接口
// ReturnCode GetInstances(const GetInstancesRequest &req, InstancesResponse *&resp);
PHP_METHOD(PolarisClient, GetInstances)
{

    // 找到对应的 C++ 实例缓存信息
    polaris_core_object *obj = (polaris_core_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

    polaris::ConsumerApi *consumer = obj->consumer;
    if (consumer != nullptr)
    {
    }
    RETURN_LONG(-1)
}

// 同步获取服务下全部服务实例，返回的实例与控制台看到的一致
// ReturnCode GetAllInstances(const GetInstancesRequest &req, InstancesResponse *&resp);
PHP_METHOD(PolarisClient, GetAllInstances)
{

    // 找到对应的 C++ 实例缓存信息
    polaris_core_object *obj = (polaris_core_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

    polaris::ConsumerApi *consumer = obj->consumer;
    if (consumer != nullptr)
    {
    }
    RETURN_LONG(-1)
}

// 上报服务调用结果，用于服务实例熔断和监控统计
// ReturnCode UpdateServiceCallResult(const ServiceCallResult &req);
PHP_METHOD(PolarisClient, UpdateServiceCallResult)
{

    // 找到对应的 C++ 实例缓存信息
    polaris_core_object *obj = (polaris_core_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

    polaris::ConsumerApi *consumer = obj->consumer;
    if (consumer != nullptr)
    {
    }
    RETURN_LONG(-1)
}

PHP_METHOD(PolarisClient, GetRouteRuleKeys)
{

    // 找到对应的 C++ 实例缓存信息
    polaris_core_object *obj = (polaris_core_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

    polaris::ConsumerApi *consumer = obj->consumer;
    if (consumer != nullptr)
    {
    }
    RETURN_LONG(-1)
}
