// Tencent is pleased to support the open source community by making polaris-go available.
//
// Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the BSD 3-Clause License (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// https://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissionsr and limitations under the License.
//

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
#include <mutex>

// 引入 Polaris ProviderApi 函数定义
#include "polaris/provider.h"
#include "polaris/consumer.h"
#include "polaris/config.h"
#include "polaris/context.h"
#include "polaris/log.h"

// 自实现
#include "provider_core.hpp"
#include "consumer_core.hpp"
#include "limit_core.hpp"
#include "common.hpp"
#include "utils.hpp"

// 定义一个 PHP 对象用于持有 ProviderApi 这个对象的实例
static zend_object_handlers polaris_core_object_handlers;

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
    // PHP_ME(PolarisClient, GetRouteRuleKeys, NULL, ZEND_ACC_PUBLIC)        /**/

    // Limit
    // PHP_ME(PolarisClient, FetchRule, NULL, ZEND_ACC_PUBLIC)          /**/
    // PHP_ME(PolarisClient, FetchRuleLabelKeys, NULL, ZEND_ACC_PUBLIC) /**/
    PHP_ME(PolarisClient, GetQuota, NULL, ZEND_ACC_PUBLIC)         /**/
    PHP_ME(PolarisClient, UpdateCallResult, NULL, ZEND_ACC_PUBLIC) /**/
    PHP_ME(PolarisClient, InitQuotaWindow, NULL, ZEND_ACC_PUBLIC)  /**/
    PHP_FE_END};

struct polaris_core_object
{
    zend_object std;
    polaris::Context *globalCtx;
    polaris::ProviderApi *provider;
    polaris::ConsumerApi *consumer;
    polaris::LimitApi *limit;
    std::mutex mtx;
};

static inline polaris_core_object *hsf_fetch_object(zend_object *obj) /* {{{ */
{
    return (polaris_core_object *)((char *)(obj)-XtOffsetOf(polaris_core_object, std));
}
/* }}} */
#define Z_USEROBJ_P(zv) hsf_fetch_object(Z_OBJ_P((zv)))

void polaris_core_free_storage(zend_object *object)
{
    polaris_core_object *obj = (polaris_core_object *)hsf_fetch_object(object);
    delete obj->provider;
    delete obj->consumer;
    delete obj->limit;
    delete obj->globalCtx;

    zend_object_std_dtor(&obj->std TSRMLS_CC);
    efree(obj);
}

zend_object *polaris_core_create_handler(zend_class_entry *cls_type TSRMLS_DC)
{
    /* Allocate memory for it */
    polaris_core_object *intern = (polaris_core_object *)ecalloc(1, sizeof(polaris_core_object) + zend_object_properties_size(cls_type));

    zend_object_std_init(&intern->std, cls_type TSRMLS_CC);
    object_properties_init(&intern->std, cls_type);

    polaris_core_object_handlers.offset = XtOffsetOf(polaris_core_object, std);
    polaris_core_object_handlers.free_obj = polaris_core_free_storage;
    intern->std.handlers = &polaris_core_object_handlers;

    return &intern->std;
}

// 定义 PolarisClient 的构造函数，这里就只支持一种吧，直接传入对应的配置文件地址用于初始化
PHP_METHOD(PolarisClient, __construct)
{
    zval *arg;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &arg))
    {
        zend_error(E_WARNING, "get parameter for config path failed");
        exit(EXIT_FAILURE);
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
        exit(EXIT_FAILURE);
    }

    zval *object = getThis();

    // 找到对应的 C++ 实例缓存信息
    polaris_core_object *obj = (polaris_core_object *)Z_USEROBJ_P(object TSRMLS_CC);
    if (obj == nullptr)
    {
        RETURN_NULL();
    }
    // static PHP_METHOD(PDO, dbh_constructor)
    obj->globalCtx = polaris::Context::Create(config, polaris::kShareContext);
    obj->provider = NULL;
    obj->consumer = NULL;
    obj->limit = NULL;

    if (obj->globalCtx == nullptr)
    {
        zend_error(E_WARNING, "create polaris SDKContext failed");
        exit(EXIT_FAILURE);
    }
}

/*
=================== ProviderApi for PHP start ===================
*/

/**
 * @brief Construct a new php method object
 *
 */
PHP_METHOD(PolarisClient, InitProvider)
{
    polaris_core_object *obj = (polaris_core_object *)Z_USEROBJ_P(getThis() TSRMLS_CC);

    if (obj == nullptr)
    {
        RETURN_NULL();
    }

    obj->mtx.lock();
    if (obj->provider == nullptr)
    {
        obj->provider = polaris::ProviderApi::Create(obj->globalCtx);
    }
    obj->mtx.unlock();
}

/**
 * @brief Construct a new php method object
 *
 */
PHP_METHOD(PolarisClient, Register)
{

    array_init(return_value);

    // 注册实例信息原始参数信息
    zval *registerVal;
    uint64_t timeout = 500;
    uint64_t flowId;

    if (ZEND_NUM_ARGS() == 3)
    {
        // 识别参数信息
        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|ll", &registerVal, &timeout, &flowId))
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

    polaris_core_object *obj = (polaris_core_object *)Z_USEROBJ_P(getThis() TSRMLS_CC);

    polaris::ProviderApi *provider = obj->provider;
    if (provider != nullptr)
    {
        polaris::ReturnCode code = RegisterInstance(provider, registerVal, timeout, flowId, return_value);
        return;
    }
    string errMsg = "polaris provider instance is nullptr";
    add_assoc_long(return_value, Code.c_str(), -1);
    add_assoc_stringl(return_value, ErrMsg.c_str(), (char *)errMsg.c_str(), errMsg.length());
    return;
}

/**
 * @brief Construct a new php method object
 *
 */
PHP_METHOD(PolarisClient, Deregister)
{
    array_init(return_value);

    // 注册实例信息原始参数信息
    zval *deregisterVal;
    uint64_t timeout = 500;
    uint64_t flowId;

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
    polaris_core_object *obj = (polaris_core_object *)Z_USEROBJ_P(getThis() TSRMLS_CC);
    polaris::ProviderApi *provider = obj->provider;
    if (provider != nullptr)
    {
        polaris::ReturnCode code = DeregisterInstance(provider, deregisterVal, timeout, flowId, return_value);
        return;
    }
    string errMsg = "polaris provider instance is nullptr";
    add_assoc_long(return_value, Code.c_str(), -1);
    add_assoc_stringl(return_value, ErrMsg.c_str(), (char *)errMsg.c_str(), errMsg.length());
    return;
}

/**
 * @brief Construct a new php method object
 *
 */
PHP_METHOD(PolarisClient, Heartbeat)
{
    array_init(return_value);

    // 实例心跳信息原始参数信息
    zval *heartbeatVal;

    if (ZEND_NUM_ARGS() == 1)
    {
        // 识别参数信息
        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &heartbeatVal))
        {
            RETURN_LONG(-1);
        }
    }
    else
    {
        WRONG_PARAM_COUNT;
        RETURN_LONG(-1);
    }
    polaris_core_object *obj = (polaris_core_object *)Z_USEROBJ_P(getThis() TSRMLS_CC);
    polaris::ProviderApi *provider = obj->provider;
    if (provider != nullptr)
    {
        polaris::ReturnCode code = DoHeartbeat(provider, heartbeatVal, return_value);
        return;
    }
    string errMsg = "polaris provider instance is nullptr";
    add_assoc_long(return_value, Code.c_str(), -1);
    add_assoc_stringl(return_value, ErrMsg.c_str(), (char *)errMsg.c_str(), errMsg.length());
    return;
}

/*
=================== ProviderApi for PHP end ===================
*/

/*
=================== LimitApi for PHP start ===================
*/

/**
 * @brief Construct a new php method object
 *
 */
PHP_METHOD(PolarisClient, InitLimit)
{
    polaris_core_object *obj = (polaris_core_object *)Z_USEROBJ_P(getThis() TSRMLS_CC);

    if (obj == nullptr)
    {
        RETURN_NULL();
    }

    obj->mtx.lock();
    if (obj->limit == nullptr)
    {
        obj->limit = polaris::LimitApi::Create(obj->globalCtx);
    }
    obj->mtx.unlock();
}

// PHP_METHOD(PolarisClient, FetchRule)
// {

//     array_init(return_value);
//     // 注册实例信息原始参数信息
//     zval *fetchVal;
//     uint64_t timeout = 500;

//     if (ZEND_NUM_ARGS() == 2)
//     {
//         // 识别参数信息
//         if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|l", &fetchVal, &timeout))
//         {
//             RETURN_LONG(-1);
//         }
//     }
//     else
//     {
//         WRONG_PARAM_COUNT;
//         RETURN_LONG(-1);
//     }

//     // 找到对应的 C++ 实例缓存信息
//     polaris_core_object *obj = (polaris_core_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
//     polaris::LimitApi *limit = obj->limit;
//     if (limit != nullptr)
//     {
//         polaris::ReturnCode code = DoFetchRule(limit, fetchVal, timeout, return_value);
//         return;
//     }
//     string errMsg = "polaris limit instance is nullptr";
//     add_assoc_long(return_value, Code.c_str(), -1);
//     add_assoc_stringl(return_value, ErrMsg.c_str(), (char *)errMsg.c_str(), errMsg.length(), 1);
//     return;
// }

// PHP_METHOD(PolarisClient, FetchRuleLabelKeys)
// {
//     array_init(return_value);
//     // 注册实例信息原始参数信息
//     zval *fetchVal;
//     uint64_t timeout = 500;

//     if (ZEND_NUM_ARGS() == 2)
//     {
//         // 识别参数信息
//         if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|l", &fetchVal, &timeout))
//         {
//             RETURN_LONG(-1);
//         }
//     }
//     else
//     {
//         WRONG_PARAM_COUNT;
//         RETURN_LONG(-1);
//     }

//     // 找到对应的 C++ 实例缓存信息
//     polaris_core_object *obj = (polaris_core_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
//     polaris::LimitApi *limit = obj->limit;
//     if (limit != nullptr)
//     {
//         polaris::ReturnCode code = DoFetchRuleLabelKeys(limit, fetchVal, timeout, return_value);
//         return;
//     }
//     string errMsg = "polaris limit instance is nullptr";
//     add_assoc_long(return_value, Code.c_str(), -1);
//     add_assoc_stringl(return_value, ErrMsg.c_str(), (char *)errMsg.c_str(), errMsg.length(), 1);
//     return;
// }

PHP_METHOD(PolarisClient, GetQuota)
{
    array_init(return_value);
    zval *quotaVal;

    if (ZEND_NUM_ARGS() == 1)
    {
        // 识别参数信息
        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &quotaVal))
        {
            RETURN_LONG(-1);
        }
    }
    else
    {
        WRONG_PARAM_COUNT;
        RETURN_LONG(-1);
    }
    polaris_core_object *obj = (polaris_core_object *)Z_USEROBJ_P(getThis() TSRMLS_CC);
    polaris::LimitApi *limit = obj->limit;
    if (limit != nullptr)
    {
        polaris::ReturnCode code = DoGetQuota(limit, quotaVal, return_value);
        return;
    }
    string errMsg = "polaris limit instance is nullptr";
    add_assoc_long(return_value, Code.c_str(), -1);
    add_assoc_stringl(return_value, ErrMsg.c_str(), (char *)errMsg.c_str(), errMsg.length());
    return;
}

PHP_METHOD(PolarisClient, UpdateCallResult)
{
    array_init(return_value);
    zval *callVal;

    if (ZEND_NUM_ARGS() == 1)
    {
        // 识别参数信息
        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &callVal))
        {
            RETURN_LONG(-1);
        }
    }
    else
    {
        WRONG_PARAM_COUNT;
        RETURN_LONG(-1);
    }

    polaris_core_object *obj = (polaris_core_object *)Z_USEROBJ_P(getThis() TSRMLS_CC);
    polaris::LimitApi *limit = obj->limit;
    if (limit != nullptr)
    {
        polaris::ReturnCode code = DoUpdateCallResult(limit, callVal, return_value);
        return;
    }
    string errMsg = "polaris limit instance is nullptr";
    add_assoc_long(return_value, Code.c_str(), -1);
    add_assoc_stringl(return_value, ErrMsg.c_str(), (char *)errMsg.c_str(), errMsg.length());
    return;
}

/**
 * @brief Construct a new php method object
 *
 */
PHP_METHOD(PolarisClient, InitQuotaWindow)
{
    array_init(return_value);
    zval *initVal;

    if (ZEND_NUM_ARGS() == 1)
    {
        // 识别参数信息
        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &initVal))
        {
            RETURN_LONG(-1);
        }
    }
    else
    {
        WRONG_PARAM_COUNT;
        RETURN_LONG(-1);
    }

    polaris_core_object *obj = (polaris_core_object *)Z_USEROBJ_P(getThis() TSRMLS_CC);
    polaris::LimitApi *limit = obj->limit;
    if (limit != nullptr)
    {
        polaris::ReturnCode code = DoInitQuotaWindow(limit, initVal, return_value);
        return;
    }
    string errMsg = "polaris limit instance is nullptr";
    add_assoc_long(return_value, Code.c_str(), -1);
    add_assoc_stringl(return_value, ErrMsg.c_str(), (char *)errMsg.c_str(), errMsg.length());
    return;
}

/*
=================== LimitApi for PHP end ===================
*/

/*
=================== ConsumerApi for PHP start ===================
*/

/**
 * @brief Construct a new php method object
 *
 */
PHP_METHOD(PolarisClient, InitConsumer)
{
    polaris_core_object *obj = (polaris_core_object *)Z_USEROBJ_P(getThis() TSRMLS_CC);

    if (obj == nullptr)
    {
        RETURN_NULL();
    }

    obj->mtx.lock();
    if (obj->consumer == nullptr)
    {
        obj->consumer = polaris::ConsumerApi::Create(obj->globalCtx);
    }
    obj->mtx.unlock();
}

/**
 * @brief Construct a new php method object
 *
 */
PHP_METHOD(PolarisClient, InitService)
{
    array_init(return_value);
    zval *initVal;
    uint64_t timeout, flowId;

    if (ZEND_NUM_ARGS() == 3)
    {
        // 识别参数信息
        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|ll", &initVal, &timeout, &flowId))
        {
            RETURN_LONG(-1);
        }
    }
    else
    {
        WRONG_PARAM_COUNT;
        RETURN_LONG(-1);
    }

    polaris_core_object *obj = (polaris_core_object *)Z_USEROBJ_P(getThis() TSRMLS_CC);
    polaris::ConsumerApi *consumer = obj->consumer;
    if (consumer != nullptr)
    {
        polaris::ReturnCode code = DoInitService(consumer, initVal, timeout, flowId, return_value);
        return;
    }
    string errMsg = "polaris consumer instance is nullptr";
    add_assoc_long(return_value, Code.c_str(), -1);
    add_assoc_stringl(return_value, ErrMsg.c_str(), (char *)errMsg.c_str(), errMsg.length());
    return;
}

/**
 * @brief Construct a new php method object
 *
 */
PHP_METHOD(PolarisClient, GetOneInstance)
{
    array_init(return_value);
    zval *reqVal;
    uint64_t timeout, flowId;

    if (ZEND_NUM_ARGS() == 3)
    {
        // 识别参数信息
        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|ll", &reqVal, &timeout, &flowId))
        {
            RETURN_LONG(-1);
        }
    }
    else
    {
        WRONG_PARAM_COUNT;
        RETURN_LONG(-1);
    }

    polaris_core_object *obj = (polaris_core_object *)Z_USEROBJ_P(getThis() TSRMLS_CC);
    polaris::ConsumerApi *consumer = obj->consumer;
    if (consumer != nullptr)
    {
        polaris::ReturnCode code = DoGetOneInstance(consumer, reqVal, timeout, flowId, return_value);
        return;
    }
    string errMsg = "polaris consumer instance is nullptr";
    add_assoc_long(return_value, Code.c_str(), -1);
    add_assoc_stringl(return_value, ErrMsg.c_str(), (char *)errMsg.c_str(), errMsg.length());
    return;
}

/**
 * @brief Construct a new php method object
 *
 */
PHP_METHOD(PolarisClient, GetInstances)
{
    array_init(return_value);
    zval *reqVal;
    uint64_t timeout, flowId;

    if (ZEND_NUM_ARGS() == 3)
    {
        // 识别参数信息
        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|ll", &reqVal, &timeout, &flowId))
        {
            RETURN_LONG(-1);
        }
    }
    else
    {
        WRONG_PARAM_COUNT;
        RETURN_LONG(-1);
    }
    polaris_core_object *obj = (polaris_core_object *)Z_USEROBJ_P(getThis() TSRMLS_CC);
    polaris::ConsumerApi *consumer = obj->consumer;
    if (consumer != nullptr)
    {
        polaris::ReturnCode code = DoGetInstances(consumer, reqVal, timeout, flowId, return_value);
        return;
    }
    string errMsg = "polaris consumer instance is nullptr";
    add_assoc_long(return_value, Code.c_str(), -1);
    add_assoc_stringl(return_value, ErrMsg.c_str(), (char *)errMsg.c_str(), errMsg.length());
    return;
}

/**
 * @brief Construct a new php method object
 *
 */
PHP_METHOD(PolarisClient, GetAllInstances)
{
    array_init(return_value);
    zval *reqVal;
    uint64_t timeout, flowId;

    if (ZEND_NUM_ARGS() == 3)
    {
        // 识别参数信息
        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|ll", &reqVal, &timeout, &flowId))
        {
            RETURN_LONG(-1);
        }
    }
    else
    {
        WRONG_PARAM_COUNT;
        RETURN_LONG(-1);
    }

    polaris_core_object *obj = (polaris_core_object *)Z_USEROBJ_P(getThis() TSRMLS_CC);
    polaris::ConsumerApi *consumer = obj->consumer;
    if (consumer != nullptr)
    {
        polaris::ReturnCode code = DoGetAllInstances(consumer, reqVal, timeout, flowId, return_value);
        return;
    }
    string errMsg = "polaris consumer instance is nullptr";
    add_assoc_long(return_value, Code.c_str(), -1);
    add_assoc_stringl(return_value, ErrMsg.c_str(), (char *)errMsg.c_str(), errMsg.length());
    return;
}

/**
 * @brief Construct a new php method object
 *
 */
PHP_METHOD(PolarisClient, UpdateServiceCallResult)
{
    array_init(return_value);
    zval *callVal;
    uint64_t timeout, flowId;

    if (ZEND_NUM_ARGS() == 3)
    {
        // 识别参数信息
        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|ll", &callVal, &timeout, &flowId))
        {
            RETURN_LONG(-1);
        }
    }
    else
    {
        WRONG_PARAM_COUNT;
        RETURN_LONG(-1);
    }

    polaris_core_object *obj = (polaris_core_object *)Z_USEROBJ_P(getThis() TSRMLS_CC);
    polaris::ConsumerApi *consumer = obj->consumer;
    if (consumer != nullptr)
    {
        polaris::ReturnCode code = DoUpdateServiceCallResult(consumer, callVal, timeout, flowId, return_value);
        return;
    }
    string errMsg = "polaris consumer instance is nullptr";
    add_assoc_long(return_value, Code.c_str(), -1);
    add_assoc_stringl(return_value, ErrMsg.c_str(), (char *)errMsg.c_str(), errMsg.length());
    return;
}

// /**
//  * @brief Construct a new php method object
//  *
//  */
// PHP_METHOD(PolarisClient, GetRouteRuleKeys)
// {
//     array_init(return_value);
//     zval *reqVal;
//     uint64_t timeout;

//     if (ZEND_NUM_ARGS() == 1)
//     {
//         // 识别参数信息
//         if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|l", &reqVal, &timeout))
//         {
//             RETURN_LONG(-1);
//         }
//     }
//     else
//     {
//         WRONG_PARAM_COUNT;
//         RETURN_LONG(-1);
//     }

//     // 找到对应的 C++ 实例缓存信息
//     polaris_core_object *obj = (polaris_core_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
//     polaris::ConsumerApi *consumer = obj->consumer;
//     if (consumer != nullptr)
//     {
//         polaris::ReturnCode code = DoGetRouteRuleKeys(consumer, reqVal, timeout, return_value);
//         return;
//     }
//     string errMsg = "polaris consumer instance is nullptr";
//     add_assoc_long(return_value, Code.c_str(), -1);
//     add_assoc_stringl(return_value, ErrMsg.c_str(), (char *)errMsg.c_str(), errMsg.length(), 1);
//     return;
// }

/*
=================== ConsumerApi for PHP end ===================
*/