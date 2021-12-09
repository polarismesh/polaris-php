# polaris-php

北极星polaris是一个支持多种开发语言、兼容主流开发框架的服务治理中心。polaris-php 是北极星的PHP语言嵌入式服务治理SDK

[English Document](./README.md)

## 概述

polaris-php 提供以下功能特性：

* ** 服务实例注册，心跳上报
   
   提供API接口供应用上下线时注册/反注册自身实例信息，并且可通过定时上报心跳来通知主调方自身健康状态。

* ** 服务发现

   提供多种API接口，通过API接口，用户可以获取服务下的全量服务实例，或者获取通过服务治理规则过滤后的一个服务实例，可供业务获取实例后马上发起调用。

* ** 故障熔断
   
   提供API接口供应用上报接口调用结果数据，并根据汇总数据快速对故障实例/分组进行隔离，以及在合适的时机进行探测恢复。

* ** 服务限流

   提供API接口供应用进行配额的检查及划扣，支持按服务级，以及接口级的限流策略。

## 快速入门

### 构建Polaris-P

可以查看此文档来进行构建polaris-php插件: [BuildDoc](./doc/HowToBuild_ZH.md)

### Using API

API的使用文档：[ApiDoc](./doc/ApiDoc_ZH.md)

### Example

如何快速使用polaris-php：[QuickStart](./examples/quickstart)
