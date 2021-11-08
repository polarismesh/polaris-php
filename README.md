# Polaris SDK FOR PHP

## Polaris Provider
### How to build


#### Build Polaris CPP

> Clone Polaris CPP

> Copy lib so
#### Build Polaris PHP

> PHP 5.6.40

```shell

cd ./polaris_provider
# clean last build info
phpize --clean 

phpize

./configure  --with-php-config=/usr/local/php56/bin/php-config  --with-polaris_provider

make && make install
```


## Polaris Consumer
### How to build

#### command

> PHP 5.6.40

```shell

cd ./polaris_consumer
# clean last build info
phpize --clean 

phpize

./configure  --with-php-config=/usr/local/php56/bin/php-config  --with-polaris_provider

make && make install
```