#ifndef _UTILS_H_
#define _UTILS_H_

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

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include "common.hpp"

static void PolarisOutputDebugString(zend_bool trigger_break, const char *format, ...) /* {{{ */
{
    va_list args;

    va_start(args, format);
#ifdef ZEND_WIN32
    {
        char output_buf[1024];

        vsnprintf(output_buf, 1024, format, args);
        OutputDebugString(output_buf);
        OutputDebugString("\n");
        if (trigger_break && IsDebuggerPresent())
        {
            DebugBreak();
        }
    }
#else
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
#endif
    va_end(args);
}

static void PolarisHashDisplay(const HashTable *ht)
{
    Bucket *p;
    uint i;

    if (UNEXPECTED(ht->nNumOfElements == 0))
    {
        PolarisOutputDebugString(0, "The hash is empty");
        return;
    }
    for (i = 0; i < ht->nNumUsed; ++i)
    {
        Bucket *b = &ht->arData[i];
        if (Z_ISUNDEF(b->val))
        {
            continue;
        }
        PolarisOutputDebugString(0, "%s <==> 0x%lX\n", b->key, b->val);
    }
}

/**
 * @brief
 *
 * @param metadata
 * @return map<string, string>
 */
static zval TransferMapToArray(map<string, string> metadata)
{
    zval metadataArr;
    array_init(&metadataArr);

    if (metadata.size() == 0)
    {
        return metadataArr;
    }

    for (map<string, string>::iterator iter = metadata.begin(); iter != metadata.end(); iter++)
    {
        // 这里直接忽略 key 为空的数据
        if (iter->first == "")
        {
            continue;
        }

        // char *valN = const_cast<char *>(iter->second.c_str());
        add_assoc_string(&metadataArr, iter->first.c_str(), iter->second.c_str());
    }

    return metadataArr;
}

static map<string, string> TransferToStdMap(HashTable *ht)
{
    if (UNEXPECTED(ht->nNumOfElements == 0) || ht == nullptr)
    {
        return map<string, string>();
    }
    map<string, string> metadata = map<string, string>();

    uint32_t i;
    for (i = 0; i < ht->nNumUsed; ++i)
    {
        Bucket *b = &ht->arData[i];
        if (Z_ISUNDEF(b->val))
        {
            continue;
        }

        metadata.insert({string(ZSTR_VAL(b->key)), string(Z_STRVAL(b->val))});
    }
    return metadata;
}

static uint getKeyLength(string val)
{
    return val.length() + 1;
}

#endif