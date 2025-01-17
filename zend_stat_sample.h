/*
  +----------------------------------------------------------------------+
  | stat                                                                 |
  +----------------------------------------------------------------------+
  | Copyright (c) Joe Watkins 2019                                       |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: krakjoe                                                      |
  +----------------------------------------------------------------------+
 */

#ifndef ZEND_STAT_SAMPLE_H
# define ZEND_STAT_SAMPLE_H

#include "zend_stat_strings.h"

#ifndef ZEND_STAT_SAMPLE_MAX_ARGINFO
#   define ZEND_STAT_SAMPLE_MAX_ARGINFO 12
#endif

typedef struct _zend_stat_sample_state_t {
    zend_bool                busy;
    zend_bool                used;
} zend_stat_sample_state_t;

typedef struct _zend_stat_sample_t {
    zend_stat_sample_state_t state;
    zend_uchar               type;
    pid_t                    pid;
    double                   elapsed;
    struct {
        size_t               used;
        size_t               peak;
    } memory;
    struct {
        zend_stat_string_t  *file;
        uint32_t             line;
        uint32_t             offset;
        zend_uchar           opcode;
    } location;
    struct {
        zend_stat_string_t  *scope;
        zend_stat_string_t  *function;
    } symbol;
    struct {
        uint32_t             length;
        zval                 info[ZEND_STAT_SAMPLE_MAX_ARGINFO];
    } arginfo;
} zend_stat_sample_t;

#define ZEND_STAT_SAMPLE_UNUSED   0
#define ZEND_STAT_SAMPLE_MEMORY   1
#define ZEND_STAT_SAMPLE_INTERNAL 2
#define ZEND_STAT_SAMPLE_USER     4

#define ZEND_STAT_SAMPLE_DATA(s) \
    (((char*) s) + XtOffsetOf(zend_stat_sample_t, type))
#define ZEND_STAT_SAMPLE_DATA_SIZE \
    (sizeof(zend_stat_sample_t) - XtOffsetOf(zend_stat_sample_t, type))

const static zend_stat_sample_t zend_stat_sample_empty = {
    {0, 0}, ZEND_STAT_SAMPLE_UNUSED, 0, 0, {0, 0}, {NULL, 0}, {NULL, NULL}
};

zend_bool zend_stat_sample_write(zend_stat_sample_t *sample, int fd);
#endif
