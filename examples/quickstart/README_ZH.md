# QuickStart

根据简单的 php socket 应用示例，演示 php 应用如何快速接入北极星。[English document](./README.md)

## 目录介绍

> consumer

php-client 端示例，负责发起 socket 请求。启动时先从北极星拉取一个对应 php-server 的服务地址并发起调用

> provider

php-server 端示例，负责处理 socket 请求。启动时进程1负责处理socket请求，进程2则进行服务实例的注册，并发送心跳维持
实例的健康状态

## 如何构建

- 构建对应的 polaris-php 插件, [构建文档](../../doc/HowToBuild_ZH.md)

## 如何使用

### 创建服务

预先通过北极星控制台创建对应的服务，如果是通过本地一键安装包的方式安装，直接在浏览器通过127.0.0.1:8091打开控制台。

![create_service](./image/create_php_service.png)

### 执行程序

运行 php-server

```shell
cd provider
php provider.php
```

运行 php-client

```shell
cd consumer
php consumer.php
```