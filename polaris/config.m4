PHP_ARG_WITH(polaris, for polaris support,
dnl Make sure that the comment is aligned:
[  --with-polaris          Include polaris support])

if test "$PHP_polaris" != "no"; then
  PHP_REQUIRE_CXX()
  PHP_ADD_LIBRARY(stdc++, 1, EXTRA_LDFLAGS)
  
  PHP_ADD_INCLUDE(../include)

  PHP_ADD_LIBRARY(stdc++, 1, POLARIS_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(polaris_api, "../lib", POLARIS_SHARED_LIBADD)
  PHP_ADD_LIBRARY(protobuf, "../lib", POLARIS_SHARED_LIBADD)

  dnl PHP_ADD_LIBRARY_WITH_PATH(polaris_api, "../lib", EXTRA_LDFLAGS)
  dnl PHP_ADD_LIBRARY_WITH_PATH(protobuf, "../lib", EXTRA_LDFLAGS)
  
  PHP_SUBST(POLARIS_SHARED_LIBADD)

  PHP_NEW_EXTENSION(polaris, polaris.cpp, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
