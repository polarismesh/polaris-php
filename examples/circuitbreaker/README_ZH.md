# CircuitBreaker Example

根据简单的 polaris-php 使用示例，演示 php 应用如何快速使用北极星的服务熔断功能。

## 如何构建

- 构建对应的 polaris-php 插件, [构建文档](../../doc/HowToBuild_ZH.md)

## 如何使用

### 创建服务

- 预先通过北极星控制台创建对应的服务，如果是通过本地一键安装包的方式安装，直接在浏览器通过127.0.0.1:8091打开控制台。
- 创建服务提供者
  - ![create_provider_service](./image/create-php-provider.png)
- 创建服务提供这实例
  - ![create_provider_instance](./image/create-php-provider-instance.png)


### 执行程序

```shell
php circuitbreaker.php
```

观察输出结果

- 输出期望值

```
array(3) {
  [0]=>
  array(19) {
    ["host"]=> string(9) "127.0.0.2"
    ["port"]=> int(8080)
    ...
  }
  [1]=>
  array(19) {
    ["host"]=> string(9) "127.0.0.1"
    ["port"]=> int(8080)
    ...
  }
  [2]=>
  array(19) {
    ["host"]=> string(9) "127.0.0.4"
    ["port"]=> int(8080)
    ...
  }
}
```
