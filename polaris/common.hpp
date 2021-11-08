#ifndef _COMMON_H_
#define _COMMON_H_

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

// Polaris 创建时的参数key名称
#define ConfigPath "config_path"
#define LogDir "log_dir"

// Polaris 一些元数据信息
#define Namespace "namespace"
#define Service "service"
#define SourceService "source"

#define Protocol "protocol"
#define Version "version"
#define Token "token"
#define Ttl "ttl"
#define Host "host"
#define Port "port"
#define Weight "weight"
#define Priority "priority"
#define Metadata "metadata"
#define SourceMetadata "source_metadata"
#define VpcID "vpc_id"
#define HeartbeatFlag "heartbeat"
#define Canary "canary"
#define IgnoreHalfOpen "ignore_half_open"
#define SourceService "source_service"
#define SourceSetName "source_set_name"
#define HashKey "hash_key"
#define HashString "hash_string"
#define Labels "labels"
#define ReplicateIndex "replicate_index"
#define BackupInstanceNum "backup_instance_num"
#define MetadataFailoverType "metadata_failover_type"
#define FlowId "flow_id"
#define Revision "revision"
#define WeightType "weigth_type"
#define ContainerName "container_name"
#define InternalSetName "internal_set_name"
#define LogicSet "logic_set"
#define Region "region"
#define Zone "zone"
#define Campus "campus"
#define LocalityAwareInfo "locality_aware_info"
#define DynamicWeight "dynamic_weight"
#define Healthy "healthy"
#define Isolate "isolate"
#define Delay "delay"
#define CallRetStatus "ret_status"
#define CallRetCode "ret_code"
#define Amount "amount"

#define Instances "instances"

#define GetResponse "response"

#define ServiceSubSet "subset"

#define RuleKeys "rule_keys"
#define LimitLabelKeys "limit_label_keys"
#define CallResponseType "response_type"
#define CallResponseTime "response_time"
#define CallResponseCode "response_code"

#define LoadBalanceType "load_balance_type"
#define kLoadBalanceTypeWeightedRandom "weightedRandom"
#define kLoadBalanceTypeRingHash "ringHash"
#define kLoadBalanceTypeMaglevHash "maglev"
#define kLoadBalanceTypeL5CstHash "l5cst"
#define kLoadBalanceTypeSimpleHash "simpleHash"
#define kLoadBalanceTypeCMurmurHash "cMurmurHash"
#define kLoadBalanceTypeLocalityAware "localityAware"
#define kLoadBalanceTypeDefaultConfig "default"

// Polaris 返回信息的一些key
#define Code "code"
#define ErrMsg "err_msg"
#define InstanceID "instance_id"

// 与限流相关的 key
#define LimitRuleJsonStr "limit_rule_json_str"
#define ResultForQuota "quota_result"
#define ResultCodeForQuota "quota_result_code"
#define ResultInfoForQuota "quota_result_info"
#define LeftQuota "quota_left"
#define AllQuota "quota_all"
#define DurationForQuota "quota_duration"
#define IsDegrade "degrade"
#define WaitTimeForQuota "wait_time"

#endif