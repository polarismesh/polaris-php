# Route Example

Based on a simple polaris-php usage example, demonstrate how a php application can quickly use the service routing function of Polaris.

## How To Build

- Build the corresponding polaris-php plugin [Build documentation](../../doc/HowToBuild.md)

## How To Use

### 创建服务

- Create the corresponding service through the Polaris console in advance. If it is installed through a local one-click installation package, open the console directly in the browser through 127.0.0.1:8080
  - Create a service provider
    - ![create_provider_service](./image/create-php-provider.png)
  - Create service provider instance
    - ![create_provider_instance](./image/create-php-provider-instance.png)
  - Create service consumers
    - ![create_provider_service](./image/create-php-consumer.png)
- Configure routing rules for service providers
  - Set up the service being called rules
    - ![setting_ratelimit](./image/create-php-provider-route-rule.png)


### Execute program

```shell
php route.php
```

Observe the output

- Output expected value

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
