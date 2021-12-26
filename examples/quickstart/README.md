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

### Build the corresponding polaris-php plugin

[Build documentation](../../doc/HowToBuild.md)

### Build quickstart docker image

- The polaris-php plugin for document building, [Build Document](../../doc/HowToBuild_ZH.md)
- Move the polaris.so file to the directory of **consumer** and **provider**
- Adjust the address of Polaris server in polaris.yaml
- Mirror construction for the provider
  - docker build --build-arg localHost={your provider ip}
## How to use

### Create Service

Create the corresponding service through the Polaris console in advance. If it is installed through a local one-click installation package, open the console directly in the browser through 127.0.0.1:8091.

![create_service](./image/create_php_service.png)

### Execute program

run php-server

```shell
cd provider
export PHP_PROVIDER_IP={your provider ip}
php provider.php
```

run php-client

```shell
cd consumer
php consumer.php
```