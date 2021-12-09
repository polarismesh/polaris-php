# How to build

## Preparation
### Build Polaris-CPP

Put the `polaris-cpp` project `clone` to a directory on your local

```shell
cd {One of your directories}

# run git clone
git clone git@github.com:polarismesh/polaris-cpp.git

# run compile build

make && make package
```

After executing make package, a polaris_cpp_sdk.tar.gz compressed file will be generated in the current directory. The content of the file is as follows：

```
|-- include/polaris  # head File
|   |-- consumer.h provider.h limit.h config.h context.h log.h defs.h ...
|-- dlib             # Dynamic library
|   |-- libpolaris_api.so
`-- slib             # Static library
    |-- libpolaris_api.a libprotobuf.a
```

Unzip the file `polaris_cpp_sdk.tar.gz`, copy `libpolaris_api.a` and `libprotobuf.a` in the `slib` directory to the `lib` directory of this project (`polaris-php`)

```shell
cd {One of your directories}/polaris-cpp
```
### Build Polaris PHP

Make sure that the local default `php` version is `5.6.x` or `7.4.x`

```shell
cd ./polaris

phpize --clean

phpize

./configure  --with-php-config=${php-config Full path information of the file}  --with-polaris_provider

make && make install
```
