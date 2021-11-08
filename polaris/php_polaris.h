/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_POLARIS_H
#define PHP_POLARIS_H

extern zend_module_entry polaris_module_entry;
#define phpext_polaris_ptr &polaris_module_entry

#define PHP_POLARIS_VERSION "0.1.0" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#define PHP_POLARIS_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#define PHP_POLARIS_API __attribute__((visibility("default")))
#else
#define PHP_POLARIS_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(polaris)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(polaris)
*/

/* In every utility function you add that needs to use variables 
   in php_polaris_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as POLARIS_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define POLARIS_G(v) TSRMG(polaris_globals_id, zend_polaris_globals *, v)
#else
#define POLARIS_G(v) (polaris_globals.v)
#endif

PHP_METHOD(PolarisClient, __construct) ;
PHP_METHOD(PolarisClient, InitProvider);
PHP_METHOD(PolarisClient, InitConsumer);
PHP_METHOD(PolarisClient, InitLimit)   ;

// Provider
PHP_METHOD(PolarisClient, Register)  ;
PHP_METHOD(PolarisClient, Deregister);
PHP_METHOD(PolarisClient, Heartbeat) ;
PHP_METHOD(PolarisClient, Heartbeat) ;

// Consumer
PHP_METHOD(PolarisClient, InitService)            ;
PHP_METHOD(PolarisClient, GetOneInstance)         ;
PHP_METHOD(PolarisClient, GetInstances)           ;
PHP_METHOD(PolarisClient, GetAllInstances)        ;
PHP_METHOD(PolarisClient, UpdateServiceCallResult);
PHP_METHOD(PolarisClient, GetRouteRuleKeys)       ;

// Limit
PHP_METHOD(PolarisClient, FetchRule)         ;
PHP_METHOD(PolarisClient, FetchRuleLabelKeys);
PHP_METHOD(PolarisClient, GetQuota)          ;
PHP_METHOD(PolarisClient, UpdateCallResult)  ;
PHP_METHOD(PolarisClient, InitQuotaWindow)   ;

#endif /* PHP_POLARIS_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
