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
#include <set>

// 引入 Polaris ProviderApi 函数定义
#include "polaris/limit.h"
#include "polaris/log.h"

#include "utils.hpp"
#include "common.hpp"

// Polaris-PHP 中的依赖

/**
 * @brief 
 * 
 * @param val 
 * @return polaris::LimitCallResultType 
 */
static polaris::LimitCallResultType convertToLimitCallResultType(string val)
{
    if (val.compare("Limit") == 0)
    {
        return polaris::kLimitCallResultLimited;
    }
    if (val.compare("Failed") == 0)
    {
        return polaris::kLimitCallResultFailed;
    }
    return polaris::kLimitCallResultOk;
}

/**
 * @brief 
 * 
 * @param reqVal 
 * @return polaris::QuotaRequest& 
 */
static polaris::QuotaRequest ConvertToQuotaRequest(zval *reqVal)
{
    map<string, string> params = TransferToStdMap(Z_ARRVAL_P(reqVal));
    map<string, string> labels = map<string, string>();

    polaris::QuotaRequest req;

    zval *labelsVal, *subsetVal;
    labelsVal = zend_hash_find(HASH_OF(reqVal), zend_string_init(Labels.c_str(), getKeyLength(Labels), 0));
    if (labelsVal != NULL)
    {
        labels = TransferToStdMap(Z_ARRVAL_P(labelsVal));
    }
    subsetVal = zend_hash_find(HASH_OF(reqVal), zend_string_init(ServiceSubSet.c_str(), getKeyLength(ServiceSubSet), 0));
    if (subsetVal != NULL)
    {
        req.SetSubset(TransferToStdMap(Z_ARRVAL_P(subsetVal)));
    }

    string tmp = params[Amount];
    if (string("").compare(tmp) == 0)
    {
        tmp = "1";
    }

    req.SetServiceNamespace(params[Namespace]);
    req.SetServiceName(params[Service]);
    req.SetAcquireAmount(atoi(tmp.c_str()));
    req.SetLabels(labels);

    return req;
}

// /**
//  * @brief
//  *
//  * @param limit
//  * @param reqVal
//  * @return polaris::ReturnCode
//  */
// static polaris::ReturnCode DoFetchRule(polaris::LimitApi *limit, zval *reqVal, uint64_t timeout, zval *returnVal)
// {
//     map<string, string> params = TransferToStdMap(Z_ARRVAL_P(reqVal));

//     polaris::ServiceKey key = {params[Namespace], params[Service]};
//     string jsonRule;

//     polaris::ReturnCode code = limit->FetchRule(key, timeout, jsonRule);

//     string errMsg = polaris::ReturnCodeToMsg(code);

//     add_assoc_long(returnVal, Code.c_str(), code);
//     add_assoc_stringl(returnVal, ErrMsg.c_str(), (char *)errMsg.c_str(), errMsg.length(), 1);
//     add_assoc_stringl(returnVal, LimitRuleJsonStr.c_str(), (char *)jsonRule.c_str(), jsonRule.length(), 1);
//     return code;
// }

// /**
//  * @brief
//  *
//  * @param limit
//  * @param reqVal
//  * @param timeout
//  * @return polaris::ReturnCode
//  */
// static polaris::ReturnCode DoFetchRuleLabelKeys(polaris::LimitApi *limit, zval *reqVal, uint64_t timeout, zval *returnVal)
// {
//     map<string, string> params = TransferToStdMap(Z_ARRVAL_P(reqVal));

//     polaris::ServiceKey key = {params[Namespace], params[Service]};

//     const set<string> *labelKeys = nullptr;
//     polaris::ReturnCode code = limit->FetchRuleLabelKeys(key, timeout, labelKeys);

//     string errMsg = polaris::ReturnCodeToMsg(code);
//     add_assoc_long(returnVal, Code.c_str(), code);
//     add_assoc_stringl(returnVal, ErrMsg.c_str(), (char *)errMsg.c_str(), errMsg.length(), 1);

//     if (code == polaris::kReturnOk && labelKeys != nullptr)
//     {
//         zval *arr;
//         ALLOC_INIT_ZVAL(arr);
//         array_init_size(arr, labelKeys->size());
//         for (set<string>::iterator iter = labelKeys->begin(); iter != labelKeys->end(); iter++)
//         {
//             std::cout << *iter << " , " << endl;
//             add_next_index_string(arr, ((string)(*iter)).c_str(), 1);
//         }
//         add_assoc_zval(returnVal, LimitLabelKeys.c_str(), arr);
//     }

//     return code;
// }

/**
 * @brief 获取配额
 * 
 * @param limit 
 * @param reqVal 获取配额请求
 * @return polaris::ReturnCode 
 * {
 *   ["quota_result"]=>
 *      array(5) {
 *        ["quota_result_code"]=>
 *        int(0)
 *        ["quota_duration"]=>
 *        int(0)
 *        ["quota_left"]=>
 *        int(0)
 *        ["quota_all"]=>
 *        int(0)
 *        ["degrade"]=>
 *        bool(false)
 *    }
 * }
 */
static polaris::ReturnCode DoGetQuota(polaris::LimitApi *limit, zval *reqVal, zval *returnVal)
{
    polaris::QuotaRequest req = ConvertToQuotaRequest(reqVal);

    polaris::QuotaResponse *resp;

    polaris::ReturnCode code = limit->GetQuota(req, resp);
    string errMsg = polaris::ReturnCodeToMsg(code);
    add_assoc_long(returnVal, Code.c_str(), code);
    add_assoc_stringl(returnVal, ErrMsg.c_str(), (char *)errMsg.c_str(), errMsg.length());

    if (code == polaris::kReturnOk && resp != nullptr)
    {
        zval *arr;
        array_init(arr);
        add_assoc_long(arr, ResultCodeForQuota.c_str(), resp->GetResultCode());
        add_assoc_long(arr, DurationForQuota.c_str(), resp->GetQuotaResultInfo().duration_);
        add_assoc_long(arr, LeftQuota.c_str(), resp->GetQuotaResultInfo().left_quota_);
        add_assoc_long(arr, AllQuota.c_str(), resp->GetQuotaResultInfo().all_quota_);
        add_assoc_bool(arr, IsDegrade.c_str(), resp->GetQuotaResultInfo().is_degrade_);
        add_assoc_long(arr, WaitTimeForQuota.c_str(), resp->GetWaitTime());
        add_assoc_zval(returnVal, ResultForQuota.c_str(), arr);
    }

    return code;
}

/**
 * @brief 更新请求配额调用结果
 * 
 * @param limit 
 * @param reqVal 
 * @param waitTime 
 * @return polaris::ReturnCode 
 */
static polaris::ReturnCode DoUpdateCallResult(polaris::LimitApi *limit, zval *reqVal, zval *returnVal)
{
    map<string, string> params = TransferToStdMap(Z_ARRVAL_P(reqVal));
    map<string, string> labels, subset;

    zval *labelsVal = zend_hash_find(HASH_OF(reqVal), zend_string_init(Labels.c_str(), getKeyLength(Labels), 0));
    if (labelsVal != NULL && Z_TYPE_P(labelsVal) == IS_ARRAY)
    {
        labels = TransferToStdMap(Z_ARRVAL_P(labelsVal));
    }
    else
    {
        labels = map<string, string>();
    }

    zval *subsetVal = zend_hash_find(HASH_OF(reqVal), zend_string_init(ServiceSubSet.c_str(), getKeyLength(ServiceSubSet), 0));
    if (subsetVal != NULL)
    {
        subset = TransferToStdMap(Z_ARRVAL_P(subsetVal));
    }
    else
    {
        subset = map<string, string>();
    }

    polaris::LimitCallResult callResult;
    callResult.SetServiceNamespace(params[Namespace]);
    callResult.SetServiceName(params[Service]);
    callResult.SetSubset(subset);
    callResult.SetLabels(labels);
    callResult.SetResponseResult(convertToLimitCallResultType(params[CallResponseType]));
    callResult.SetResponseTime(atol(params[CallResponseTime].c_str()));
    callResult.SetResponseCode(atoi(params[CallResponseCode].c_str()));

    polaris::ReturnCode code = limit->UpdateCallResult(callResult);
    string errMsg = polaris::ReturnCodeToMsg(code);
    add_assoc_long(returnVal, Code.c_str(), code);
    add_assoc_stringl(returnVal, ErrMsg.c_str(), (char *)errMsg.c_str(), errMsg.length());
    return code;
}

/**
 * @brief 初始化配额窗口，可选调用，用于提前初始化配窗口减小首次配额延迟
 * 
 * @param limit 
 * @param reqVal 
 * @param waitTime 
 * @return polaris::ReturnCode 
 */
static polaris::ReturnCode DoInitQuotaWindow(polaris::LimitApi *limit, zval *reqVal, zval *returnVal)
{
    polaris::QuotaRequest req = ConvertToQuotaRequest(reqVal);

    polaris::ReturnCode code = limit->InitQuotaWindow(req);
    string errMsg = polaris::ReturnCodeToMsg(code);
    add_assoc_long(returnVal, Code.c_str(), code);
    add_assoc_stringl(returnVal, ErrMsg.c_str(), (char *)errMsg.c_str(), errMsg.length());
    return code;
}