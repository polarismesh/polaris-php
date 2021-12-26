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

#ifndef _COMMON_POLARIS_PHP_H_
#define _COMMON_POLARIS_PHP_H_

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
string ConfigPath = "config_path";
string LogDir = "log_dir";

// Polaris 一些元数据信息
string Namespace = "namespace";
string Service = "service";
string Protocol = "protocol";
string Version = "version";
string Token = "token";
string Ttl = "ttl";
string Host = "host";
string Port = "port";
string Weight = "weight";
string Priority = "priority";
string Metadata = "metadata";
string SourceMetadata = "source_metadata";
string VpcID = "vpc_id";
string HeartbeatFlag = "heartbeat";
string Canary = "canary";
string IgnoreHalfOpen = "ignore_half_open";
string IncludeUnhealthyInstances = "include_unhealthy_instances";
string IncludeCircuitBreakInstances = "include_circuit_breaker_instances";
string SkipRouteFilter = "skip_route_filter";
string SourceService = "source_service";
string SourceSetName = "source_set_name";
string HashKey = "hash_key";
string HashString = "hash_string";
string Labels = "labels";
string ReplicateIndex = "replicate_index";
string BackupInstanceNum = "backup_instance_num";
string MetadataFailoverTypeStr = "metadata_failover_type";
string FlowId = "flow_id";
string Revision = "revision";
string WeightTypeStr = "weigth_type";
string ContainerName = "container_name";
string InternalSetName = "internal_set_name";
string LogicSet = "logic_set";
string Region = "region";
string Zone = "zone";
string Campus = "campus";
string LocalityAwareInfo = "locality_aware_info";
string DynamicWeight = "dynamic_weight";
string Healthy = "healthy";
string Isolate = "isolate";
string Delay = "delay";
string CallRetStatus = "ret_status";
string CallRetCode = "ret_code";
string Amount = "amount";
string Instances = "instances";
string GetResponse = "response";
string ServiceSubSet = "subset";
string RuleKeys = "rule_keys";
string LimitLabelKeys = "limit_label_keys";
string CallResponseType = "response_type";
string CallResponseTime = "response_time";
string CallResponseCode = "response_code";
string LoadBalanceTypeStr = "load_balance_type";
string kLoadBalanceTypeWeightedRandomStr = "weightedRandom";
string kLoadBalanceTypeRingHashStr = "ringHash";
string kLoadBalanceTypeMaglevHashStr = "maglev";
string kLoadBalanceTypeL5CstHashStr = "l5cst";
string kLoadBalanceTypeSimpleHashStr = "simpleHash";
string kLoadBalanceTypeCMurmurHashStr = "cMurmurHash";
string kLoadBalanceTypeLocalityAwareStr = "localityAware";
string kLoadBalanceTypeDefaultConfigStr = "default";
// Polaris 返回信息的一些key
string Code = "code";
string ErrMsg = "err_msg";
string InstanceID = "instance_id";

// 与限流相关的 key
string LimitRuleJsonStr = "limit_rule_json_str";
string ResultForQuota = "quota_result";
string ResultCodeForQuota = "quota_result_code";
string ResultInfoForQuota = "quota_result_info";
string LeftQuota = "quota_left";
string AllQuota = "quota_all";
string DurationForQuota = "quota_duration";
string IsDegrade = "degrade";
string WaitTimeForQuota = "wait_time";

#endif