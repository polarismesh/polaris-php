# How to build

## 前期准备
### 构建 Polaris-CPP

将`polaris-cpp`项目`clone`到你本地的一个目录中

```shell
cd {你的一个目录}
# 下载
git clone git@github.com:polarismesh/polaris-cpp.git

# 执行编译构建

make && make package
```

执行make package后会在当前目录下生成一个polaris_cpp_sdk.tar.gz压缩文件。该文件的内容如下：

```
|-- include/polaris  # 头文件
|   |-- consumer.h provider.h limit.h config.h context.h log.h defs.h ...
|-- dlib             # 动态库
|   |-- libpolaris_api.so
`-- slib             # 静态库
    |-- libpolaris_api.a libprotobuf.a
```

对文件`polaris_cpp_sdk.tar.gz`进行解压，将`slib`目录下的`libpolaris_api.a`以及`libprotobuf.a`拷贝到本项目(`polaris-php`)的`lib`目录下

```shell
cd {你的一个目录}/polaris-cpp
```

最终的目录结构如下

```shell
.
├── doc
│   ├── ApiDoc.md
│   └── HowToBuild.md
├── example
│   ├── polaris_consumer.php
│   ├── polaris_limit.php
│   └── polaris_provider.php
├── include
│   ├── polaris
│   └── utils.hpp
├── lib
│   ├── libpolaris_api.a
│   └── libprotobuf.a
├── php
│   └── php-5.6.40
├── polaris
├── polaris.yaml
├── polaris.yaml.template
└── README.md
```

### 构建 Polaris PHP

确保本地默认的`php`版本为`5.6.x`或者为`7.4.x`版本

```shell

cd ./polaris
# clean last build info
phpize --clean

phpize

./configure  --with-php-config=${php-config 文件的全路径信息}  --with-polaris_provider

make && make install
```
