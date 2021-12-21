FROM php:7.4-cli

# 确保可以编辑 polaris.yaml
RUN apt-get update && apt-get install -y vim

RUN docker-php-ext-install \
    pcntl \
    sockets

COPY polaris.so /usr/local/lib/php/extensions/no-debug-non-zts-20190902/polaris.so

COPY provider.php /root/provider.php
COPY polaris.yaml /root/polaris.yaml

# 本机 IP 信息
ARG localHost=127.0.0.1

ENV PHP_PROVIDER_IP=$localHost

EXPOSE 9996

WORKDIR /root

CMD ["php", "provider.php"]