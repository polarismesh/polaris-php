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

    if (UNEXPECTED(ht->nNumOfElements == 0) || ht == nullptr)
    {
        PolarisOutputDebugString(0, "The hash is empty");
        return;
    }
    for (i = 0; i < ht->nTableSize; i++)
    {
        p = ht->arBuckets[i];
        while (p != NULL)
        {
            PolarisOutputDebugString(0, "%s <==> 0x%lX\n", p->arKey, p->h);
            p = p->pNext;
        }
    }
}

/**
 * @brief 
 * 
 * @param metadata 
 * @return map<string, string> 
 */
static zval *TransferMapToArray(map<string, string> metadata)
{
    zval *metadataArr;
    ALLOC_INIT_ZVAL(metadataArr);
    array_init(metadataArr);

    for (map<string, string>::iterator iter = metadata.begin(); iter != metadata.end(); iter++)
    {
        char *valN = const_cast<char *>(iter->second.c_str());
        add_assoc_string(metadataArr, iter->first.c_str(), valN, 1);
    }

    return metadataArr;
}

static map<string, string> TransferToStdMap(HashTable *ht)
{
    if (UNEXPECTED(ht->nNumOfElements == 0) || ht == nullptr)
    {
        return map<string, string>();
    }
    Bucket *p;

    map<string, string> metadata = map<string, string>();

    p = ht->pListTail;
    while (p != nullptr)
    {

        zval **data = ((zval **)(p->pData));
        // convert_to_string_ex(data);
        metadata.insert({string(p->arKey), string(Z_STRVAL_PP(data))});

        Bucket *p_last = p->pListLast;
        p = p_last;
    }

    return metadata;
}

#endif