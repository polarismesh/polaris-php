#!/bin/bash

rm -rf /root/corefile/core-*

test_module=$1

/usr/local/php56/bin/phpize --clean

/usr/local/php56/bin/phpize

./configure --with-php-config=/usr/local/php56/bin/php-config --with-polaris

make && make install

if [[ "${test_module}" == "provider" ]]; then
    /usr/local/php56/bin/php ../example/polaris_provider.php
    exit $?
fi

if [[ "${test_module}" == "limit" ]]; then
    /usr/local/php56/bin/php ../example/polaris_limit.php
    exit $?
fi

if [[ "${test_module}" == "consumer" ]]; then
    /usr/local/php56/bin/php ../example/polaris_consumer.php
    exit $?
fi

if [[ "${test_module}" == "all" ]]; then
    /usr/local/php56/bin/php ../example/polaris_provider.php
    /usr/local/php56/bin/php ../example/polaris_limit.php
    /usr/local/php56/bin/php ../example/polaris_consumer.php
fi
