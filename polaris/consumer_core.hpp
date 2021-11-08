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
#include "polaris/consumer.h"
#include "polaris/log.h"

#include "common.hpp"
#include "utils.hpp"

// 负载均衡映射关系
static map<string, polaris::LoadBalanceType> _loadBalanceTypeMap = {
    {kLoadBalanceTypeWeightedRandom, polaris::kLoadBalanceTypeWeightedRandom},
    {kLoadBalanceTypeRingHash, polaris::kLoadBalanceTypeRingHash},
    {kLoadBalanceTypeMaglevHash, polaris::kLoadBalanceTypeMaglevHash},
    {kLoadBalanceTypeL5CstHash, polaris::kLoadBalanceTypeL5CstHash},
    {kLoadBalanceTypeSimpleHash, polaris::kLoadBalanceTypeSimpleHash},
    {kLoadBalanceTypeCMurmurHash, polaris::kLoadBalanceTypeCMurmurHash},
    {kLoadBalanceTypeLocalityAware, polaris::kLoadBalanceTypeLocalityAware},
    {kLoadBalanceTypeDefaultConfig, polaris::kLoadBalanceTypeDefaultConfig},
};

static polaris::CallRetStatus convertToCallRetStatus(string val)
{
    if (string("error").compare(val))
    {
        return polaris::kCallRetError;
    }
    if (string("timeout").compare(val))
    {
        return polaris::kCallRetTimeout;
    }
    return polaris::kCallRetOk;
}

/**
 * @brief 将 polaris::Instance 转换为 php 的 array
 * 
 * @param inst 
 * @return zval* 
 */
static zval *convertInstanceToArray(polaris::Instance inst)
{
    zval *arr;
    ALLOC_INIT_ZVAL(arr);
    array_init(arr);

    add_assoc_string(arr, Host, inst.GetHost());
    add_assoc_string(arr, ContainerName, inst.GetContainerName());
    add_assoc_string(arr, InternalSetName, inst.GetInternalSetName());
    add_assoc_string(arr, LogicSet, inst.GetLogicSet());
    add_assoc_string(arr, Region, inst.GetRegion());
    add_assoc_string(arr, Zone, inst.GetZone());
    add_assoc_string(arr, Campus, inst.GetCampus());
    add_assoc_string(arr, VpcID, inst.GetVpcId());
    add_assoc_string(arr, Protocol, inst.GetProtocol());
    add_assoc_string(arr, Version, inst.GetVersion());

    add_assoc_long(arr, Port, inst.GetPort());
    add_assoc_long(arr, Weight, inst.GetWeight());
    add_assoc_long(arr, Priority, inst.GetPriority());
    add_assoc_long(arr, DynamicWeight, inst.GetDynamicWeight());
    add_assoc_long(arr, HashKey, inst.GetHash());
    add_assoc_long(arr, LocalityAwareInfo, inst.GetLocalityAwareInfo());

    add_assoc_bool(arr, Healthy, inst.isHealthy());
    add_assoc_bool(arr, Isolate, inst.isIsolate());

    add_assoc_zval(arr, Metadata, TransferMapToArray(inst.GetMetadata()));

    return arr;
}

static string convertWeigthTypeForString(polaris::WeightType wt)
{
    if (wt == polaris::kStaticWeightType)
    {
        return "static";
    }
    return "dynamic"
}

static polaris::MetadataFailoverType convertToMetadataFailoverType(string val)
{
    if ("notKey".compare(val))
    {
        return polaris::kMetadataFailoverNotKey;
    }
    if ("all".compare(val))
    {
        return polaris::kMetadataFailoverAll;
    }
    return polaris::kMetadataFailoverNone;
}

static polaris::GetOneInstanceRequest &convertoToGetOneInstanceRequest(zval *val, uint64_t timeout, uint64_t flowId)
{
    // 整体的参数描述信息
    map<string, string> params = TransferToStdMap(Z_ARRVAL_P(reqVal));

    zval **labelsVal, **metadataVal, **srcmetaVal, **sourceServiceVal;
    map<string, string> labels = map<string, string>();
    map<string, string> matadata = map<string, string>();
    map<string, string> sourceMetadata = map<string, string>();
    map<string, string> sourceService = map<string, string>();

    polaris::GetOneInstanceRequest req({params[Namespace], params[Service]});
    req.SetFlowId(flowId);
    req.SetTimeout(timeout);
    req.SetCanary(params[Canary]);
    req.SetSourceSetName(SourceSetName);
    req.SetIgnoreHalfOpen(string("true").compare(params[IgnoreHalfOpen]));
    req.SetHashString(params[HashString]);
    req.SetHashKey(params[HashKey]);
    req.SetReplicateIndex(atoi(params[ReplicateIndex]));
    req.SetBackupInstanceNum(atoi(params[BackupInstanceNum]));
    req.SetLoadBalanceType(_loadBalanceTypeMap[params[LoadBalanceType]]);
    req.SetMetadataFailover(convertToMetadataFailoverType(params[MetadataFailoverType]));

    // 获取被调用服务的元数据信息
    if (zend_hash_find(HASH_OF(reqVal), Metadata, sizeof(Metadata), (void **)(&metadataVal)) == SUCCESS)
    {
        matadata = TransferToStdMap(Z_ARRVAL_PP(metadataVal));
    }
    req.SetMetadata(matadata);
    // 获取被调用服务的元数据信息
    if (zend_hash_find(HASH_OF(reqVal), Labels, sizeof(Labels), (void **)(&labelsVal)) == SUCCESS)
    {
        labels = TransferToStdMap(Z_ARRVAL_PP(metadataVal));
    }
    req.SetLabels(labels);

    if (zend_hash_find(HASH_OF(reqVal), SourceService, sizeof(SourceService), (void **)(&sourceServiceVal)) == SUCCESS)
    {
        // 获取被调服务的信息数据
        sourceService = TransferToStdMap(Z_ARRVAL_PP(sourceServiceVal));
        // 获取被调服务的 metadata 数据
        if (zend_hash_find(HASH_OF(*sourceServiceVal), Metadata, sizeof(Metadata), (void **)(&srcmetaVal)) == SUCCESS)
        {
            sourceMetadata = TransferToStdMap(Z_ARRVAL_PP(srcmetaVal));
        }

        polaris::ServiceInfo srcInfo;
        srcInfo.metadata_ = sourceMetadata;
        srcInfo.service_key_.name_ = sourceService[Service];
        srcInfo.service_key_.namespace_ = sourceService[Namespace];
        req.SetSourceService(srcInfo);
    }

    return req;
}

static zval *convertServiceResponseToZval(polaris::InstancesResponse *resp)
{
    zval *arr;
    ALLOC_INIT_ZVAL(arr);
    array_init(arr);

    // 设置服务的基本信息数据
    add_assoc_long(arr, FlowId, resp->GetFlowId());
    add_assoc_string(arr, Service, resp->GetServiceName());
    add_assoc_string(arr, Namespace, resp->GetServiceNamespace());
    add_assoc_string(arr, Revision, resp->GetRevision());
    add_assoc_zval(arr, Metadata, TransferMapToArray(resp->GetMetadata()));
    add_assoc_string(arr, WeightType, convertWeigthTypeForString(resp->GetWeightType()));
    add_assoc_zval(arr, ServiceSubSet, TransferMapToArray(resp->GetSubset()));

    // 设置服务实例的基本信息数据
    zval *instancesVal;
    ALLOC_INIT_ZVAL(instancesVal);
    array_init(instancesVal);

    vector<polaris::Instance> instances = resp->GetInstances();

    // 将 polaris::Instance[] 装载到 php array 中
    for (int i = 0; i < instances.size(); i++)
    {
        polaris::Instance inst = instances[i];
        zval *val = convertInstanceToArray(inst);
        add_next_index_zval(instancesVal, val);
    }

    add_assoc_zval(arr, Instances, instancesVal);

    return arr;
}

/**
 * @brief 用于提前初始化服务数据
 * 
 * @param consumer 
 * @param reqVal 
 * @param timeout 
 * @param flowId 
 * @return polaris::ReturnCode 
 */
static polaris::ReturnCode DoInitService(polaris::ConsumerApi *consumer, zval *reqVal, uint64_t timeout, uint64_t flowId, zval *returnVal)
{
    polaris::GetOneInstanceRequest req = convertoToGetOneInstanceRequest(reqVal);
    req.SetFlowId(flowId);
    req.SetTimeout(timeout);

    polaris::ReturnCode code = consumer->InitService(req);
    string errMsg = polaris::ReturnCodeToMsg(code);
    add_assoc_long(returnVal, Code, code);
    add_assoc_stringl(returnVal, ErrMsg, (char *)errMsg.c_str(), errMsg.length(), 1);
    return code;
}

/**
 * @brief 同步获取单个服务实例
 * 
 * @param consumer 
 * @param reqVal 
 * @param timeout 
 * @param flowId 
 * @return polaris::ReturnCode 
 */
static polaris::ReturnCode DoGetOneInstance(polaris::ConsumerApi *consumer, zval *reqVal, uint64_t timeout, uint64_t flowId, zval *returnVal)
{
    polaris::GetOneInstanceRequest req = convertoToGetOneInstanceRequest(reqVal);
    req.SetFlowId(flowId);
    req.SetTimeout(timeout);

    polaris::InstancesResponse *resp;

    polaris::ReturnCode code = consumer->GetOneInstance(req, resp);

    if (code == polaris::kReturnOk)
    {
        zval *arr = convertServiceResponseToZval(resp);
        add_assoc_zval(returnVal, GetResponse, arr);
    }
    string errMsg = polaris::ReturnCodeToMsg(code);
    add_assoc_long(returnVal, Code, code);
    add_assoc_stringl(returnVal, ErrMsg, (char *)errMsg.c_str(), errMsg.length(), 1);
    return code;
}

/**
 * @brief 同步获取批量服务实例
 * @note 该接口不会返回熔断半开实例，实例熔断后，进入半开如何没有请求一段时间后会自动恢复
 * 
 * @param consumer 
 * @param reqVal 
 * @param timeout 
 * @param flowId 
 * @return polaris::ReturnCode 
 */
static polaris::ReturnCode DoGetInstances(polaris::ConsumerApi *consumer, zval *reqVal, uint64_t timeout, uint64_t flowId, zval *returnVal)
{
    polaris::GetOneInstanceRequest req = convertoToGetOneInstanceRequest(reqVal);
    req.SetFlowId(flowId);
    req.SetTimeout(timeout);

    polaris::InstancesResponse *resp;

    polaris::ReturnCode code = consumer->GetInstances(req, resp);

    if (code == polaris::kReturnOk)
    {
        zval *arr = convertServiceResponseToZval(resp);
        add_assoc_zval(returnVal, GetResponse, arr);
    }
    string errMsg = polaris::ReturnCodeToMsg(code);
    add_assoc_long(returnVal, Code, code);
    add_assoc_stringl(returnVal, ErrMsg, (char *)errMsg.c_str(), errMsg.length(), 1);
    return code;
}

/**
 * @brief 同步获取服务下全部服务实例，返回的实例与控制台看到的一致
 * 
 * @param consumer 
 * @param reqVal 
 * @param timeout 
 * @param flowId 
 * @return polaris::ReturnCode 
 */
static polaris::ReturnCode DoGetAllInstances(polaris::ConsumerApi *consumer, zval *reqVal, uint64_t timeout, uint64_t flowId, zval *returnVal)
{
    polaris::GetOneInstanceRequest req = convertoToGetOneInstanceRequest(reqVal);
    req.SetFlowId(flowId);
    req.SetTimeout(timeout);

    polaris::InstancesResponse *resp;

    polaris::ReturnCode code = consumer->DoGetAllInstances(req, resp);

    if (code == polaris::kReturnOk)
    {
        zval *arr = convertServiceResponseToZval(resp);
        add_assoc_zval(returnVal, GetResponse, arr);
    }
    string errMsg = polaris::ReturnCodeToMsg(code);
    add_assoc_long(returnVal, Code, code);
    add_assoc_stringl(returnVal, ErrMsg, (char *)errMsg.c_str(), errMsg.length(), 1);
    return code;
}

/**
 * @brief 上报服务调用结果，用于服务实例熔断和监控统计
 * @note 本调用没有网络操作，只是将数据写入内存
 * 
 * @param consumer 
 * @param val 
 * @return polaris::ReturnCode 
 */
static polaris::ReturnCode DoUpdateServiceCallResult(polaris::ConsumerApi *consumer, zval *val, uint64_t timeout, uint64_t flowId, zval *returnVal)
{
    // 整体的参数描述信息
    map<string, string> params = TransferToStdMap(Z_ARRVAL_P(reqVal));

    zval **labelsVal, **subsetVal, **sourceVal;
    map<string, string> labels = map<string, string>();
    map<string, string> subset = map<string, string>();

    polaris::ServiceCallResult result;
    result.SetFlowId(flowId);
    result.SetTimeout(timeout);
    result.SetServiceName(params[Service]);
    result.SetServiceNamespace(params[Namespace]);
    result.SetInstanceId(params[InstanceID]);
    result.SetInstanceHostAndPort(params[Host], atoi(params[Port]));
    result.SetDelay(atol(params[Delay]));
    result.SetLocalityAwareInfo(atol(params[LocalityAwareInfo]));
    result.SetRetStatus(convertToCallRetStatus(params[CallRetStatus]));
    result.SetRetCode(atoi(params[CallRetCode]));

    // 获取被调用服务的元数据信息
    if (zend_hash_find(HASH_OF(reqVal), ServiceSubSet, sizeof(ServiceSubSet), (void **)(&subsetVal)) == SUCCESS)
    {
        subset = TransferToStdMap(Z_ARRVAL_PP(subsetVal));
    }
    result.SetSubset(subset);
    // 获取被调用服务的元数据信息
    if (zend_hash_find(HASH_OF(reqVal), Labels, sizeof(Labels), (void **)(&labelsVal)) == SUCCESS)
    {
        labels = TransferToStdMap(Z_ARRVAL_PP(labelsVal));
    }
    result.SetLabels(labels);

    // 获取主调服务的信息数据
    if (zend_hash_find(HASH_OF(reqVal), SourceService, sizeof(SourceService), (void **)(&sourceVal)) == SUCCESS)
    {
        map<string, string> sourceSvr = TransferToStdMap(Z_ARRVAL_PP(sourceVal));
        polaris::ServiceKey key = {sourceSvr[Namespace], sourceSvr[Service]};
        result.SetSource(key);
    }

    polaris::ReturnCode code = consumer->UpdateServiceCallResult(result);
    string errMsg = polaris::ReturnCodeToMsg(code);
    add_assoc_long(returnVal, Code, code);
    add_assoc_stringl(returnVal, ErrMsg, (char *)errMsg.c_str(), errMsg.length(), 1);
    return code;
}

/**
 * @brief 拉取路由规则配置的所有key
 * 
 * @param consumer 
 * @param val 
 * @param timeout 
 * @return polaris::ReturnCode 
 */
static polaris::ReturnCode DoGetRouteRuleKeys(polaris::ConsumerApi *consumer, zval *val, uint64_t timeout, zval *returnVal)
{
    map<string, string> params = TransferToStdMap(Z_ARRVAL_P(reqVal));

    polaris::ServiceKey key = {params[Namespace], params[Service]};

    const set<string> *ruleKeys = nullptr;
    polaris::ReturnCode code = consumer->GetRouteRuleKeys(key, timeout, ruleKeys);

    string errMsg = polaris::ReturnCodeToMsg(code);
    add_assoc_long(returnVal, Code, code);
    add_assoc_stringl(returnVal, ErrMsg, (char *)errMsg.c_str(), errMsg.length(), 1);

    if (code == polaris::kReturnOk && ruleKeys != nullptr)
    {
        zval *arr;
        ALLOC_INIT_ZVAL(arr);
        array_init_size(arr, ruleKeys->size());
        for (set<string>::iterator iter = ruleKeys->begin(); iter != ruleKeys->end(); iter++)
        {
            std::cout << *iter << " , " << endl;
            add_next_index_string(arr, ((string)(*iter)).c_str(), 1);
        }
        add_assoc_zval(returnVal, RuleKeys, arr);
    }

    return code;
}