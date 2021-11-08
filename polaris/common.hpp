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
#define Protocol "protocol"
#define Version "version"
#define Token "token"
#define Ttl "ttl"
#define Host "host"
#define Port "port"
#define Weight "weight"
#define Priority "priority"
#define Metadata "metadata"
#define VpcID "vpc_id"
#define HeartbeatFlag "heartbeat"

// Polaris 返回信息的一些key
#define ErrMsg "err_msg"

#endif