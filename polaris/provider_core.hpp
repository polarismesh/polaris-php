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

static polaris::ReturnCode RegisterInstance(polaris::ProviderApi *provider, zval *reqVal, zval *metadataVal, uint64_t timeout, uint64_t flowId)
{
    map<string, string> params = TransferToStdMap(Z_ARRVAL_P(reqVal));
    map<string, string> metadata = TransferToStdMap(Z_ARRVAL_P(metadataVal));

    int port = atoi(params[Port].c_str());
    int weight = atoi(params[Weight].c_str());
    int priority = atoi(params[Priority].c_str());
    int ttl = atoi(params[Ttl].c_str());
    bool healthcheck = params[HeartbeatFlag] == "true";
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
    add_assoc_stringl(reqVal, "instance_id", (char *)instanceId.c_str(), instanceId.length(), 1);

    return code;
}