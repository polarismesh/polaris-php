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
#include <map>

// 引入 Polaris ProviderApi 函数定义
#include "polaris/provider.h"
#include "polaris/log.h"

#include "utils.hpp"
#include "common.hpp"

/**
 * @brief 
 * 
 * @param provider 
 * @param reqVal 
 * @param metadataVal 
 * @param timeout 
 * @param flowId 
 * @return polaris::ReturnCode 
 */
static polaris::ReturnCode RegisterInstance(polaris::ProviderApi *provider, zval *reqVal, uint64_t timeout, uint64_t flowId, zval *returnVal)
{
    map<string, string> params = TransferToStdMap(Z_ARRVAL_P(reqVal));
    map<string, string> metadata = map<string, string>();

    zval *metadataVal;
    metadataVal = zend_hash_find(HASH_OF(reqVal), zend_string_init(Metadata.c_str(), getKeyLength(Metadata), 0));
    if (metadataVal != NULL)
    {
        metadata = TransferToStdMap(Z_ARRVAL_P(metadataVal));
    }

    int port = atoi(params[Port].c_str());
    int weight = atoi(params[Weight].c_str());
    int priority = atoi(params[Priority].c_str());
    bool healthcheck = string("true").compare(params[HeartbeatFlag]) == 0;
    int ttl = 0;
    if (healthcheck)
    {
        ttl = atoi(params[Ttl].c_str());
    }

    string tmp = params[Token];
    string token = tmp.empty() ? "polaris_php_sdk__" + params[Service] : tmp;

    // 获取 metadata 数据
    // 进行 InstanceRegisterRequest 请求参数的初始化动作
    polaris::InstanceRegisterRequest req(params[Namespace], params[Service], token, params[Host], port);

    req.SetTimeout(timeout);
    req.SetProtocol(params[Protocol]);
    req.SetVpcId(params[VpcID]);
    req.SetVersion(params[Version]);
    req.SetMetadata(metadata);
    req.SetHealthCheckFlag(healthcheck);
    req.SetTtl(ttl);
    req.SetWeight(weight);
    req.SetFlowId(flowId);
    req.SetPriority(priority);

    string instanceId;
    polaris::ReturnCode code = provider->Register(req, instanceId);
    string errMsg = polaris::ReturnCodeToMsg(code);
    add_assoc_long(returnVal, Code.c_str(), code);
    add_assoc_stringl(returnVal, ErrMsg.c_str(), (char *)errMsg.c_str(), errMsg.length());
    add_assoc_stringl(returnVal, InstanceID.c_str(), (char *)instanceId.c_str(), instanceId.length());
    return code;
}

/**
 * @brief 
 * 
 * @param provider 
 * @param reqVal 
 * @param timeout 
 * @param flowId 
 * @return polaris::ReturnCode 
 */
static polaris::ReturnCode DeregisterInstance(polaris::ProviderApi *provider, zval *reqVal, uint64_t timeout, uint64_t flowId, zval *returnVal)
{
    // 将参数转换为 map<string, string> 对象
    map<string, string> params = TransferToStdMap(Z_ARRVAL_P(reqVal));

    int port = atoi(params[Port].c_str());
    string tmp = params[Token];
    string token = tmp.empty() ? "polaris_php_sdk__" + params[Service] : tmp;

    // 进行 InstanceDeregisterRequest 请求参数的初始化动作
    polaris::InstanceDeregisterRequest req(params[Namespace], params[Service], token, params[Host], port);
    req.SetTimeout(timeout);
    req.SetFlowId(flowId);
    req.SetVpcId(params[VpcID]);

    polaris::ReturnCode code = provider->Deregister(req);
    // 返回值回插入到 register_req 中？
    string errMsg = polaris::ReturnCodeToMsg(code);
    add_assoc_long(returnVal, Code.c_str(), code);
    add_assoc_stringl(returnVal, ErrMsg.c_str(), (char *)errMsg.c_str(), errMsg.length());
    return code;
}

/**
 * @brief 
 * 
 * @param provider 
 * @param reqVal 
 * @return polaris::ReturnCode 
 */
static polaris::ReturnCode DoHeartbeat(polaris::ProviderApi *provider, zval *reqVal, zval *returnVal)
{
    // 将参数转换为 map<string, string> 对象
    map<string, string> params = TransferToStdMap(Z_ARRVAL_P(reqVal));

    int port = atoi(params[Port].c_str());
    string tmp = params[Token];
    string token = tmp.empty() ? "polaris_php_sdk__" + params[Service] : tmp;

    polaris::InstanceHeartbeatRequest req(params[Namespace], params[Service], token, params[Host], port);
    req.SetVpcId(params[VpcID]);

    polaris::ReturnCode code = provider->Heartbeat(req);
    // 返回值回插入到 register_req 中？
    string errMsg = polaris::ReturnCodeToMsg(code);
    add_assoc_long(returnVal, Code.c_str(), code);
    add_assoc_stringl(returnVal, ErrMsg.c_str(), (char *)errMsg.c_str(), errMsg.length());
    return code;
}