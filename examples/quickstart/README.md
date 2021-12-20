# QuickStart

Know a simple php socket application example, demonstrating how php application can quickly connect to Polaris. [中文文档](./README_ZH.md)

## Catalog Introduction

> consumer

An example of php-client, responsible for initiating socket requests. At startup, first pull a service address corresponding to php-server from Polaris and initiate a call

> provider


php-server example, responsible for processing socket requests. At startup, process 1 is responsible for processing
socket requests, and process 2 registers the service instance and sends a heartbeat to maintain the health status of the
instance

## How to build

- Build the corresponding polaris-php plugin. [Build documentation](../../doc/HowToBuild.md)

## How to use

### Create Service

Create the corresponding service through the Polaris console in advance. If it is installed through a local one-click installation package, open the console directly in the browser through 127.0.0.1:8091.

![create_service](./image/create_php_service.png)

### Execute program

run php-server

```shell
cd provider
php provider.php
```

run php-client

```shell
cd consumer
php consumer.php
```