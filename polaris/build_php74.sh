#!/bin/bash

rm -rf /root/corefile/core-*

test_module=$1

/usr/local/php74/bin/phpize --clean

/usr/local/php74/bin/phpize

./configure --with-php-config=/usr/local/php74/bin/php-config --with-polaris

make && make install
