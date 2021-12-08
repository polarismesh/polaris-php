#!/bin/bash

rm -rf /root/corefile/core-*

test_module=$1

/usr/local/php74/bin/phpize --clean

/usr/local/php74/bin/phpize

./configure --with-php-config=/usr/local/php74/bin/php-config --with-polaris

make && make install

cur_dir=$(pwd)

if [[ "${test_module}" == "provider" ]]; then
    cd ../examples/quickstart/provider && /usr/local/php74/bin/php provider.php
    exit $?
fi

if [[ "${test_module}" == "consumer" ]]; then
    cd ../examples/quickstart/consumer && /usr/local/php74/bin/php consumer.php
    exit $?
fi
if [[ "${test_module}" == "limit" ]]; then
    cd ../examples/ratelimit && /usr/local/php74/bin/php ratelimit.php
    exit $?
fi
