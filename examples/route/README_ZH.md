# Route Example

根据简单的 polaris-php 使用示例，演示 php 应用如何快速使用北极星的服务路由功能。

## 如何构建

- 构建对应的 polaris-php 插件, [构建文档](../../doc/HowToBuild_ZH.md)

## 如何使用

### 创建服务

- 预先通过北极星控制台创建对应的服务，如果是通过本地一键安装包的方式安装，直接在浏览器通过127.0.0.1:8091打开控制台。
  - 创建服务提供者
    - ![create_provider_service](./image/create-php-provider.png)
  - 创建服务提供这实例
    - ![create_provider_instance](./image/create-php-provider-instance.png)
  - 创建服务消费者
    - ![create_provider_service](./image/create-php-consumer.png)
- 配置服务提供者的路由规则
  - 设置服务的被调规则
    - ![setting_ratelimit](./image/create-php-provider-route-rule.png)


### 执行程序

```shell
php route.php
```

观察输出结果

- 输出期望值

```
array(1) {
  [0]=>
  array(19) {
    ["host"]=> string(9) "127.0.0.4"
    ["container_name"]=> string(0) ""
    ["internal_set_name"]=> string(0) ""
    ["logic_set"]=> string(0) ""
    ["region"]=> string(0) ""
    ["zone"]=> string(0) ""
    ["campus"]=> string(0) ""
    ["vpc_id"]=> string(0) ""
    ["protocol"]=> string(0) ""
    ["version"]=> string(0) ""
    ["port"]=> int(8080)
    ["weight"]=> int(100)
    ["priority"]=> int(0)
    ["dynamic_weight"]=> int(100)
    ["hash_key"]=> int(2926897704236617270)
    ["locality_aware_info"]=> int(0)
    ["healthy"]=> ool(true)
    ["isolate"]=> bool(false)
    ["metadata"]=> array(3) {
      ["env"]=> string(3) "pre"
      ["protocol"]=> string(0) ""
      ["version"]=> string(0) ""
    }
  }
}
```
