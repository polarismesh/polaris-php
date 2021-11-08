using namespace std;

extern "C"
{
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_polaris.h"
}

#include <string>
#include <iostream>
#include <vector>

// 引入 Polaris ProviderApi 函数定义
#include "polaris/limit.h"
#include "polaris/log.h"

// Polaris-PHP 中的依赖
