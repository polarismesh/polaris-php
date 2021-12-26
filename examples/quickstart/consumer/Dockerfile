FROM php:7.4-cli

# 确保可以编辑 polaris.yaml
RUN apt-get update && apt-get install -y vim

RUN docker-php-ext-install \
    pcntl \
    sockets

COPY polaris.so /usr/local/lib/php/extensions/no-debug-non-zts-20190902/polaris.so

COPY consumer.php /root/consumer.php
COPY polaris.yaml /root/polaris.yaml

EXPOSE 18090

WORKDIR /root

CMD ["php", "consumer.php"]