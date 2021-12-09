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

static polaris::LoadBalanceType ConvertToLoadBalanceType(string val)
{
    if (kLoadBalanceTypeWeightedRandomStr.compare(val) == 0)
        return polaris::kLoadBalanceTypeWeightedRandom;
    if (kLoadBalanceTypeRingHashStr.compare(val) == 0)
        return polaris::kLoadBalanceTypeRingHash;
    if (kLoadBalanceTypeMaglevHashStr.compare(val) == 0)
        return polaris::kLoadBalanceTypeMaglevHash;
    if (kLoadBalanceTypeL5CstHashStr.compare(val) == 0)
        return polaris::kLoadBalanceTypeL5CstHash;
    if (kLoadBalanceTypeSimpleHashStr.compare(val) == 0)
        return polaris::kLoadBalanceTypeSimpleHash;
    if (kLoadBalanceTypeCMurmurHashStr.compare(val) == 0)
        return polaris::kLoadBalanceTypeCMurmurHash;
    if (kLoadBalanceTypeLocalityAwareStr.compare(val) == 0)
        return polaris::kLoadBalanceTypeLocalityAware;
    if (kLoadBalanceTypeDefaultConfigStr.compare(val) == 0)
        return polaris::kLoadBalanceTypeDefaultConfig;
    return polaris::kLoadBalanceTypeWeightedRandom;
}

static polaris::CallRetStatus convertToCallRetStatus(string val)
{
    if (string("error").compare(val) == 0)
    {
        return polaris::kCallRetError;
    }
    if (string("timeout").compare(val) == 0)
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

    zval *retVal;
    zval arr;
    array_init(&arr);
    retVal = &arr;

    char *hostN = const_cast<char *>(inst.GetHost().c_str());
    char *containerNameN = const_cast<char *>(inst.GetContainerName().c_str());
    char *internalSetNameN = const_cast<char *>(inst.GetInternalSetName().c_str());
    char *logicSetN = const_cast<char *>(inst.GetLogicSet().c_str());
    char *regionN = const_cast<char *>(inst.GetRegion().c_str());
    char *zoneN = const_cast<char *>(inst.GetZone().c_str());
    char *campusN = const_cast<char *>(inst.GetCampus().c_str());
    char *vpcIdN = const_cast<char *>(inst.GetVpcId().c_str());
    char *protocolN = const_cast<char *>(inst.GetProtocol().c_str());
    char *versionN = const_cast<char *>(inst.GetVersion().c_str());


    add_assoc_string(retVal, Host.c_str(), hostN);
    add_assoc_string(retVal, ContainerName.c_str(), containerNameN);
    add_assoc_string(retVal, InternalSetName.c_str(), internalSetNameN);
    add_assoc_string(retVal, LogicSet.c_str(), logicSetN);
    add_assoc_string(retVal, Region.c_str(), regionN);
    add_assoc_string(retVal, Zone.c_str(), zoneN);
    add_assoc_string(retVal, Campus.c_str(), campusN);
    add_assoc_string(retVal, VpcID.c_str(), vpcIdN);
    add_assoc_string(retVal, Protocol.c_str(), protocolN);
    add_assoc_string(retVal, Version.c_str(), versionN);

    add_assoc_long(retVal, Port.c_str(), inst.GetPort());
    add_assoc_long(retVal, Weight.c_str(), inst.GetWeight());
    add_assoc_long(retVal, Priority.c_str(), inst.GetPriority());
    add_assoc_long(retVal, DynamicWeight.c_str(), inst.GetDynamicWeight());
    add_assoc_long(retVal, HashKey.c_str(), inst.GetHash());
    add_assoc_long(retVal, LocalityAwareInfo.c_str(), inst.GetLocalityAwareInfo());
    add_assoc_bool(retVal, Healthy.c_str(), inst.isHealthy());
    add_assoc_bool(retVal, Isolate.c_str(), inst.isIsolate());
    add_assoc_zval(retVal, Metadata.c_str(), TransferMapToArray(inst.GetMetadata()));
    return retVal;
}

static string ConvertWeigthTypeForString(polaris::WeightType wt)
{
    if (wt == polaris::kStaticWeightType)
    {
        return "static";
    }
    return "dynamic";
}

static polaris::MetadataFailoverType ConvertToMetadataFailoverType(string val)
{
    if (string("notKey").compare(val) == 0)
    {
        return polaris::kMetadataFailoverNotKey;
    }
    if (string("all").compare(val) == 0)
    {
        return polaris::kMetadataFailoverAll;
    }
    return polaris::kMetadataFailoverNone;
}

static polaris::GetOneInstanceRequest *convertoToGetOneInstanceRequest(zval *reqVal, uint64_t timeout, uint64_t flowId)
{
    // 整体的参数描述信息
    map<string, string> params = TransferToStdMap(Z_ARRVAL_P(reqVal));

    map<string, string> labels = map<string, string>();
    map<string, string> matadata = map<string, string>();
    map<string, string> sourceMetadata = map<string, string>();
    map<string, string> sourceService = map<string, string>();

    polaris::ServiceKey service_key = {params[Namespace], params[Service]};
    polaris::GetOneInstanceRequest req(service_key);
    req.SetFlowId(flowId);
    req.SetTimeout(timeout);
    req.SetCanary(params[Canary]);
    req.SetSourceSetName(SourceSetName);
    req.SetIgnoreHalfOpen(string("true").compare(params[IgnoreHalfOpen]) == 0);
    req.SetHashString(params[HashString]);
    req.SetHashKey(atol(params[HashKey].c_str()));
    req.SetReplicateIndex(atoi(params[ReplicateIndex].c_str()));
    req.SetBackupInstanceNum(atoi(params[BackupInstanceNum].c_str()));
    req.SetLoadBalanceType(ConvertToLoadBalanceType(params[LoadBalanceTypeStr]));
    req.SetMetadataFailover(ConvertToMetadataFailoverType(params[MetadataFailoverTypeStr]));

    zval *labelsVal, *metadataVal, *srcmetaVal, *sourceServiceVal;

    // 获取被调用服务的元数据信息
    metadataVal = zend_hash_find(HASH_OF(reqVal), zend_string_init(Metadata.c_str(), getKeyLength(Metadata), 0));
    if (metadataVal != NULL)
    {
        matadata = TransferToStdMap(Z_ARRVAL_P(metadataVal));
    }
    req.SetMetadata(matadata);
    // 获取被调用服务的元数据信息
    labelsVal = zend_hash_find(HASH_OF(reqVal), zend_string_init(Labels.c_str(), getKeyLength(Labels), 0));
    if (labelsVal != NULL)
    {
        labels = TransferToStdMap(Z_ARRVAL_P(metadataVal));
    }
    req.SetLabels(labels);

    sourceServiceVal = zend_hash_find(HASH_OF(reqVal), zend_string_init(SourceService.c_str(), getKeyLength(SourceService), 0));
    if (sourceServiceVal != NULL)
    {
        // 获取被调服务的信息数据
        sourceService = TransferToStdMap(Z_ARRVAL_P(sourceServiceVal));
        // 获取被调服务的 metadata 数据
        srcmetaVal = zend_hash_find(HASH_OF(sourceServiceVal), zend_string_init(Metadata.c_str(), getKeyLength(Metadata), 0));
        if (srcmetaVal != NULL)
        {
            sourceMetadata = TransferToStdMap(Z_ARRVAL_P(srcmetaVal));
        }

        polaris::ServiceInfo srcInfo;
        srcInfo.metadata_ = sourceMetadata;
        srcInfo.service_key_.name_ = sourceService[Service];
        srcInfo.service_key_.namespace_ = sourceService[Namespace];
        req.SetSourceService(srcInfo);
    }

    return &req;
}

static polaris::GetInstancesRequest *convertoToGetInstancesRequest(zval *reqVal, uint64_t timeout, uint64_t flowId)
{
    // 整体的参数描述信息
    map<string, string> params = TransferToStdMap(Z_ARRVAL_P(reqVal));

    map<string, string> labels = map<string, string>();
    map<string, string> matadata = map<string, string>();
    map<string, string> sourceMetadata = map<string, string>();
    map<string, string> sourceService = map<string, string>();

    polaris::ServiceKey service_key = {params[Namespace], params[Service]};
    polaris::GetInstancesRequest req(service_key);
    req.SetFlowId(flowId);
    req.SetTimeout(timeout);
    req.SetIncludeUnhealthyInstances(string("true").compare(params[IncludeUnhealthyInstances]) == 0);
    req.SetIncludeCircuitBreakInstances(string("true").compare(params[IncludeCircuitBreakInstances]) == 0);
    req.SetSkipRouteFilter(string("true").compare(params[SkipRouteFilter]) == 0);
    req.SetCanary(params[Canary]);
    req.SetSourceSetName(params[SourceSetName]);
    req.SetMetadataFailover(ConvertToMetadataFailoverType(params[MetadataFailoverTypeStr]));


    zval *labelsVal, *metadataVal, *srcmetaVal, *sourceServiceVal;

    // 获取被调用服务的元数据信息
    metadataVal = zend_hash_find(HASH_OF(reqVal), zend_string_init(Metadata.c_str(), getKeyLength(Metadata), 0));
    if (metadataVal != NULL)
    {
        matadata = TransferToStdMap(Z_ARRVAL_P(metadataVal));
    }
    req.SetMetadata(matadata);
    sourceServiceVal = zend_hash_find(HASH_OF(reqVal), zend_string_init(SourceService.c_str(), getKeyLength(SourceService), 0));
    if (sourceServiceVal != NULL)
    {
        // 获取被调服务的信息数据
        sourceService = TransferToStdMap(Z_ARRVAL_P(sourceServiceVal));
        // 获取被调服务的 metadata 数据
        srcmetaVal = zend_hash_find(HASH_OF(sourceServiceVal), zend_string_init(Metadata.c_str(), getKeyLength(Metadata), 0));
        if (srcmetaVal != NULL)
        {
            sourceMetadata = TransferToStdMap(Z_ARRVAL_P(srcmetaVal));
        }

        polaris::ServiceInfo srcInfo;
        srcInfo.metadata_ = sourceMetadata;
        srcInfo.service_key_.name_ = sourceService[Service];
        srcInfo.service_key_.namespace_ = sourceService[Namespace];
        req.SetSourceService(srcInfo);
    }

    return &req;
}

static zval *convertServiceResponseToZval(polaris::InstancesResponse *resp)
{
    zval *arr;
    zval tmpArr;
    array_init(&tmpArr);
    arr = &tmpArr;

    // 设置服务的基本信息数据
    add_assoc_long(arr, FlowId.c_str(), resp->GetFlowId());

    char *svrN = const_cast<char *>(resp->GetServiceName().c_str());
    char *nsN = const_cast<char *>(resp->GetServiceNamespace().c_str());
    char *revN = const_cast<char *>(resp->GetRevision().c_str());
    char *weightTypeN = const_cast<char *>(ConvertWeigthTypeForString(resp->GetWeightType()).c_str());


    add_assoc_string(arr, Service.c_str(), svrN);
    add_assoc_string(arr, Namespace.c_str(), nsN);
    add_assoc_string(arr, Revision.c_str(), revN);
    add_assoc_string(arr, WeightTypeStr.c_str(), weightTypeN);
    add_assoc_zval(arr, Metadata.c_str(), TransferMapToArray(resp->GetMetadata()));
    add_assoc_zval(arr, ServiceSubSet.c_str(), TransferMapToArray(resp->GetSubset()));

    zval *instancesVal;
// 设置服务实例的基本信息数据
    zval tmpVal;
    array_init(&tmpVal);
    instancesVal = &tmpVal;

    vector<polaris::Instance> instances = resp->GetInstances();

    // 将 polaris::Instance[] 装载到 php array 中
    for (int i = 0; i < instances.size(); i++)
    {
        polaris::Instance inst = instances[i];
        zval *val = convertInstanceToArray(inst);
        add_next_index_zval(instancesVal, val);
    }

    add_assoc_zval(arr, Instances.c_str(), instancesVal);

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
    // polaris::GetOneInstanceRequest req = *convertoToGetOneInstanceRequest(reqVal, timeout, flowId);
    // 整体的参数描述信息
    map<string, string> params = TransferToStdMap(Z_ARRVAL_P(reqVal));

    map<string, string> labels = map<string, string>();
    map<string, string> matadata = map<string, string>();
    map<string, string> sourceMetadata = map<string, string>();
    map<string, string> sourceService = map<string, string>();

    polaris::ServiceKey service_key = {params[Namespace], params[Service]};
    polaris::GetOneInstanceRequest req(service_key);
    req.SetFlowId(flowId);
    req.SetTimeout(timeout);
    req.SetCanary(params[Canary]);
    req.SetSourceSetName(SourceSetName);
    req.SetIgnoreHalfOpen(string("true").compare(params[IgnoreHalfOpen]) == 0);
    req.SetHashString(params[HashString]);
    req.SetHashKey(atol(params[HashKey].c_str()));
    req.SetReplicateIndex(atoi(params[ReplicateIndex].c_str()));
    req.SetBackupInstanceNum(atoi(params[BackupInstanceNum].c_str()));
    req.SetLoadBalanceType(ConvertToLoadBalanceType(params[LoadBalanceTypeStr]));
    req.SetMetadataFailover(ConvertToMetadataFailoverType(params[MetadataFailoverTypeStr]));

    zval *labelsVal, *metadataVal, *srcmetaVal, *sourceServiceVal;

    // 获取被调用服务的元数据信息
    metadataVal = zend_hash_find(HASH_OF(reqVal), zend_string_init(Metadata.c_str(), getKeyLength(Metadata), 0));
    if (metadataVal != NULL)
    {
        matadata = TransferToStdMap(Z_ARRVAL_P(metadataVal));
    }
    req.SetMetadata(matadata);
    // 获取被调用服务的元数据信息
    labelsVal = zend_hash_find(HASH_OF(reqVal), zend_string_init(Labels.c_str(), getKeyLength(Labels), 0));
    if (labelsVal != NULL)
    {
        labels = TransferToStdMap(Z_ARRVAL_P(metadataVal));
    }
    req.SetLabels(labels);

    sourceServiceVal = zend_hash_find(HASH_OF(reqVal), zend_string_init(SourceService.c_str(), getKeyLength(SourceService), 0));
    if (sourceServiceVal != NULL)
    {
        // 获取被调服务的信息数据
        sourceService = TransferToStdMap(Z_ARRVAL_P(sourceServiceVal));
        // 获取被调服务的 metadata 数据
        srcmetaVal = zend_hash_find(HASH_OF(sourceServiceVal), zend_string_init(Metadata.c_str(), getKeyLength(Metadata), 0));
        if (srcmetaVal != NULL)
        {
            sourceMetadata = TransferToStdMap(Z_ARRVAL_P(srcmetaVal));
        }
        polaris::ServiceInfo srcInfo;
        srcInfo.metadata_ = sourceMetadata;
        srcInfo.service_key_.name_ = sourceService[Service];
        srcInfo.service_key_.namespace_ = sourceService[Namespace];
        req.SetSourceService(srcInfo);
    }

    polaris::ReturnCode code = consumer->InitService(req);
    string errMsg = polaris::ReturnCodeToMsg(code);
    add_assoc_long(returnVal, Code.c_str(), code);
    add_assoc_stringl(returnVal, ErrMsg.c_str(), (char *)errMsg.c_str(), errMsg.length());
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
    // polaris::GetOneInstanceRequest req = *convertoToGetOneInstanceRequest(reqVal, timeout, flowId);
    map<string, string> params = TransferToStdMap(Z_ARRVAL_P(reqVal));

    map<string, string> labels = map<string, string>();
    map<string, string> matadata = map<string, string>();
    map<string, string> sourceMetadata = map<string, string>();
    map<string, string> sourceService = map<string, string>();

    polaris::ServiceKey service_key = {params[Namespace], params[Service]};
    polaris::GetOneInstanceRequest req(service_key);
    req.SetFlowId(flowId);
    req.SetTimeout(timeout);
    req.SetCanary(params[Canary]);
    req.SetSourceSetName(SourceSetName);
    req.SetIgnoreHalfOpen(string("true").compare(params[IgnoreHalfOpen]) == 0);
    req.SetHashString(params[HashString]);
    req.SetHashKey(atol(params[HashKey].c_str()));
    req.SetReplicateIndex(atoi(params[ReplicateIndex].c_str()));
    req.SetBackupInstanceNum(atoi(params[BackupInstanceNum].c_str()));
    req.SetLoadBalanceType(ConvertToLoadBalanceType(params[LoadBalanceTypeStr]));
    req.SetMetadataFailover(ConvertToMetadataFailoverType(params[MetadataFailoverTypeStr]));

    zval *labelsVal, *metadataVal, *srcmetaVal, *sourceServiceVal;
    // 获取被调用服务的元数据信息
    metadataVal = zend_hash_find(HASH_OF(reqVal), zend_string_init(Metadata.c_str(), getKeyLength(Metadata), 0));
    if (metadataVal != NULL)
    {
        matadata = TransferToStdMap(Z_ARRVAL_P(metadataVal));
    }
    req.SetMetadata(matadata);
    // 获取被调用服务的元数据信息
    labelsVal = zend_hash_find(HASH_OF(reqVal), zend_string_init(Labels.c_str(), getKeyLength(Labels), 0));
    if (labelsVal != NULL)
    {
        labels = TransferToStdMap(Z_ARRVAL_P(metadataVal));
    }
    req.SetLabels(labels);
    sourceServiceVal = zend_hash_find(HASH_OF(reqVal), zend_string_init(SourceService.c_str(), getKeyLength(SourceService), 0));
    if (sourceServiceVal != NULL)
    {
        // 获取被调服务的信息数据
        sourceService = TransferToStdMap(Z_ARRVAL_P(sourceServiceVal));
        // 获取被调服务的 metadata 数据
        srcmetaVal = zend_hash_find(HASH_OF(sourceServiceVal), zend_string_init(Metadata.c_str(), getKeyLength(Metadata), 0));
        if (srcmetaVal != NULL)
        {
            sourceMetadata = TransferToStdMap(Z_ARRVAL_P(srcmetaVal));
        }

        polaris::ServiceInfo srcInfo;
        srcInfo.metadata_ = sourceMetadata;
        srcInfo.service_key_.name_ = sourceService[Service];
        srcInfo.service_key_.namespace_ = sourceService[Namespace];
        req.SetSourceService(srcInfo);
    }

    polaris::InstancesResponse *resp;

    polaris::ReturnCode code = consumer->GetOneInstance(req, resp);

    if (code == polaris::kReturnOk)
    {
        zval *arr = convertServiceResponseToZval(resp);
        add_assoc_zval(returnVal, GetResponse.c_str(), arr);
    }
    string errMsg = polaris::ReturnCodeToMsg(code);
    add_assoc_long(returnVal, Code.c_str(), code);
    add_assoc_stringl(returnVal, ErrMsg.c_str(), (char *)errMsg.c_str(), errMsg.length());
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
    // polaris::GetInstancesRequest req = *convertoToGetInstancesRequest(reqVal, timeout, flowId);
    map<string, string> params = TransferToStdMap(Z_ARRVAL_P(reqVal));

    map<string, string> labels = map<string, string>();
    map<string, string> matadata = map<string, string>();
    map<string, string> sourceMetadata = map<string, string>();
    map<string, string> sourceService = map<string, string>();

    polaris::ServiceKey service_key = {params[Namespace], params[Service]};
    polaris::GetInstancesRequest req(service_key);
    req.SetFlowId(flowId);
    req.SetTimeout(timeout);
    req.SetIncludeUnhealthyInstances(string("true").compare(params[IncludeUnhealthyInstances]) == 0);
    req.SetIncludeCircuitBreakInstances(string("true").compare(params[IncludeCircuitBreakInstances]) == 0);
    req.SetSkipRouteFilter(string("true").compare(params[SkipRouteFilter]) == 0);
    req.SetCanary(params[Canary]);
    req.SetSourceSetName(params[SourceSetName]);
    req.SetMetadataFailover(ConvertToMetadataFailoverType(params[MetadataFailoverTypeStr]));

    zval *labelsVal, *metadataVal, *srcmetaVal, *sourceServiceVal;

    // 获取被调用服务的元数据信息
    metadataVal = zend_hash_find(HASH_OF(reqVal), zend_string_init(Metadata.c_str(), getKeyLength(Metadata), 0));
    if (metadataVal != NULL)
    {
        matadata = TransferToStdMap(Z_ARRVAL_P(metadataVal));
    }
    req.SetMetadata(matadata);
    sourceServiceVal = zend_hash_find(HASH_OF(reqVal), zend_string_init(SourceService.c_str(), getKeyLength(SourceService), 0));
    if (sourceServiceVal != NULL)
    {
        // 获取被调服务的信息数据
        sourceService = TransferToStdMap(Z_ARRVAL_P(sourceServiceVal));
        // 获取被调服务的 metadata 数据
        srcmetaVal = zend_hash_find(HASH_OF(reqVal), zend_string_init(Metadata.c_str(), getKeyLength(Metadata), 0));
        if (srcmetaVal != NULL)
        {
            sourceMetadata = TransferToStdMap(Z_ARRVAL_P(srcmetaVal));
        }

        polaris::ServiceInfo srcInfo;
        srcInfo.metadata_ = sourceMetadata;
        srcInfo.service_key_.name_ = sourceService[Service];
        srcInfo.service_key_.namespace_ = sourceService[Namespace];
        req.SetSourceService(srcInfo);
    }

    polaris::InstancesResponse *resp;

    polaris::ReturnCode code = consumer->GetInstances(req, resp);

    if (code == polaris::kReturnOk)
    {
        zval *arr = convertServiceResponseToZval(resp);
        add_assoc_zval(returnVal, GetResponse.c_str(), arr);
    }
    string errMsg = polaris::ReturnCodeToMsg(code);
    add_assoc_long(returnVal, Code.c_str(), code);
    add_assoc_stringl(returnVal, ErrMsg.c_str(), (char *)errMsg.c_str(), errMsg.length());
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
    // polaris::GetInstancesRequest req = *convertoToGetInstancesRequest(reqVal, timeout, flowId);
    map<string, string> params = TransferToStdMap(Z_ARRVAL_P(reqVal));

    map<string, string> labels = map<string, string>();
    map<string, string> matadata = map<string, string>();
    map<string, string> sourceMetadata = map<string, string>();
    map<string, string> sourceService = map<string, string>();

    polaris::ServiceKey service_key = {params[Namespace], params[Service]};
    polaris::GetInstancesRequest req(service_key);
    req.SetFlowId(flowId);
    req.SetTimeout(timeout);
    req.SetIncludeUnhealthyInstances(string("true").compare(params[IncludeUnhealthyInstances]) == 0);
    req.SetIncludeCircuitBreakInstances(string("true").compare(params[IncludeCircuitBreakInstances]) == 0);
    req.SetSkipRouteFilter(string("true").compare(params[SkipRouteFilter]) == 0);
    req.SetCanary(params[Canary]);
    req.SetSourceSetName(params[SourceSetName]);
    req.SetMetadataFailover(ConvertToMetadataFailoverType(params[MetadataFailoverTypeStr]));

    zval *labelsVal, *metadataVal, *srcmetaVal, *sourceServiceVal;

    // 获取被调用服务的元数据信息
    metadataVal = zend_hash_find(HASH_OF(reqVal), zend_string_init(Metadata.c_str(), getKeyLength(Metadata), IS_STR_PERMANENT));
    if (metadataVal != NULL)
    {
        matadata = TransferToStdMap(Z_ARRVAL_P(metadataVal));
    }
    req.SetMetadata(matadata);
    sourceServiceVal = zend_hash_find(HASH_OF(reqVal), zend_string_init(SourceService.c_str(), getKeyLength(SourceService), IS_STR_PERMANENT));
    if (sourceServiceVal != NULL)
    {
        // 获取被调服务的信息数据
        sourceService = TransferToStdMap(Z_ARRVAL_P(sourceServiceVal));
        // 获取被调服务的 metadata 数据
        srcmetaVal = zend_hash_find(HASH_OF(sourceServiceVal), zend_string_init(Metadata.c_str(), getKeyLength(Metadata), IS_STR_PERMANENT));
        if (srcmetaVal != NULL)
        {
            sourceMetadata = TransferToStdMap(Z_ARRVAL_P(srcmetaVal));
        }
        polaris::ServiceInfo srcInfo;
        srcInfo.metadata_ = sourceMetadata;
        srcInfo.service_key_.name_ = sourceService[Service];
        srcInfo.service_key_.namespace_ = sourceService[Namespace];
        req.SetSourceService(srcInfo);
    }

    polaris::InstancesResponse *resp;
    polaris::ReturnCode code = consumer->GetAllInstances(req, resp);
    if (code == polaris::kReturnOk)
    {
        zval *arr = convertServiceResponseToZval(resp);
        add_assoc_zval(returnVal, GetResponse.c_str(), arr);
    }
    string errMsg = polaris::ReturnCodeToMsg(code);
    add_assoc_long(returnVal, Code.c_str(), code);
    add_assoc_stringl(returnVal, ErrMsg.c_str(), (char *)errMsg.c_str(), errMsg.length());
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
static polaris::ReturnCode DoUpdateServiceCallResult(polaris::ConsumerApi *consumer, zval *reqVal, uint64_t timeout, uint64_t flowId, zval *returnVal)
{
    // 整体的参数描述信息
    map<string, string> params = TransferToStdMap(Z_ARRVAL_P(reqVal));

    map<string, string> labels = map<string, string>();
    map<string, string> subset = map<string, string>();

    polaris::ServiceCallResult result;
    result.SetServiceName(params[Service]);
    result.SetServiceNamespace(params[Namespace]);
    result.SetInstanceId(params[InstanceID]);
    result.SetInstanceHostAndPort(params[Host], atoi(params[Port].c_str()));
    result.SetDelay(atol(params[Delay].c_str()));
    result.SetLocalityAwareInfo(atol(params[LocalityAwareInfo].c_str()));
    result.SetRetStatus(convertToCallRetStatus(params[CallRetStatus]));
    result.SetRetCode(atoi(params[CallRetCode].c_str()));

    zval *labelsVal, *subsetVal, *sourceVal;
    subsetVal = zend_hash_find(HASH_OF(reqVal), zend_string_init(ServiceSubSet.c_str(), getKeyLength(ServiceSubSet), 0));
    // 获取被调用服务的元数据信息
    if (subsetVal != NULL)
    {
        subset = TransferToStdMap(Z_ARRVAL_P(subsetVal));
    }
    result.SetSubset(subset);
    // 获取被调用服务的元数据信息
    labelsVal = zend_hash_find(HASH_OF(reqVal), zend_string_init(Labels.c_str(), getKeyLength(Labels), 0));
    if (labelsVal != NULL)
    {
        labels = TransferToStdMap(Z_ARRVAL_P(labelsVal));
    }
    result.SetLabels(labels);
    // 获取主调服务的信息数据
    sourceVal = zend_hash_find(HASH_OF(reqVal), zend_string_init(SourceService.c_str(), getKeyLength(SourceService), 0));
    if (sourceVal != NULL)
    {
        map<string, string> sourceSvr = TransferToStdMap(Z_ARRVAL_P(sourceVal));
        polaris::ServiceKey key = {sourceSvr[Namespace], sourceSvr[Service]};
        result.SetSource(key);
    }

    polaris::ReturnCode code = consumer->UpdateServiceCallResult(result);
    string errMsg = polaris::ReturnCodeToMsg(code);
    add_assoc_long(returnVal, Code.c_str(), code);
    add_assoc_stringl(returnVal, ErrMsg.c_str(), (char *)errMsg.c_str(), errMsg.length());
    return code;
}

// /**
//  * @brief 拉取路由规则配置的所有key
//  *
//  * @param consumer
//  * @param val
//  * @param timeout
//  * @return polaris::ReturnCode
//  */
// static polaris::ReturnCode DoGetRouteRuleKeys(polaris::ConsumerApi *consumer, zval *reqVal, uint64_t timeout, zval *returnVal)
// {
//     map<string, string> params = TransferToStdMap(Z_ARRVAL_P(reqVal));

//     polaris::ServiceKey key = {params[Namespace], params[Service]};

//     const set<string> *ruleKeys = nullptr;
//     polaris::ReturnCode code = consumer->GetRouteRuleKeys(key, timeout, ruleKeys);

//     string errMsg = polaris::ReturnCodeToMsg(code);
//     add_assoc_long(returnVal, Code.c_str(), code);
//     add_assoc_stringl(returnVal, ErrMsg.c_str(), (char *)errMsg.c_str(), errMsg.length(), 1);

//     if (code == polaris::kReturnOk && ruleKeys != nullptr)
//     {
//         zval *arr;
//         ALLOC_INIT_ZVAL(arr);
//         array_init_size(arr, ruleKeys->size());
//         for (set<string>::iterator iter = ruleKeys->begin(); iter != ruleKeys->end(); iter++)
//         {
//             std::cout << *iter << " , " << endl;
//             add_next_index_string(arr, ((string)(*iter)).c_str(), 1);
//         }
//         add_assoc_zval(returnVal, RuleKeys.c_str(), arr);
//     }

//     return code;
// }