# CircuitBreaker Example

Based on a simple polaris-php usage example, demonstrate how a php application can quickly use Polaris' service CircuitBreaker.

## How to build

- Build the corresponding polaris-php plugin [Build documentation](../../doc/HowToBuild.md)

## How to Use

### Create Service

- Create the corresponding service through the Polaris console in advance. If it is installed through a local one-click installation package, open the console directly in the browser through 127.0.0.1:8091.
- Create a service provider
  - ![create_provider_service](./image/create-php-provider.png)
- Create service provider instance
  - ![create_provider_instance](./image/create-php-provider-instance.png)


### Execute program

```shell
php circuitbreaker.php
```

Observe the output

- Output expected value

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