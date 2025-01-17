/*
  +----------------------------------------------------------------------+
  | stat                                                                |
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

#ifndef ZEND_STAT_IO_H
# define ZEND_STAT_IO_H

#include "zend_stat_buffer.h"
#include "zend_stat_strings.h"

#include <pthread.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>

typedef enum {
    ZEND_STAT_IO_UNKNOWN,
    ZEND_STAT_IO_UNIX,
    ZEND_STAT_IO_TCP,
    ZEND_STAT_IO_FAILED
} zend_stat_io_type_t;

typedef struct _zend_stat_io_t zend_stat_io_t;

typedef void (zend_stat_io_routine_t) (zend_stat_io_t *io, int client);

struct _zend_stat_io_t {
    zend_stat_io_type_t     type;
    int                     descriptor;
    struct sockaddr         *address;
    zend_bool               closed;
    pthread_t               thread;
    zend_stat_buffer_t      *buffer;
    zend_stat_io_routine_t  *routine;
};

zend_bool zend_stat_io_write(int fd, char *message, size_t length);
zend_bool zend_stat_io_write_string(int fd, zend_stat_string_t *string);
zend_bool zend_stat_io_write_int(int fd, zend_long num);
zend_bool zend_stat_io_write_double(int fd, double num);

#define zend_stat_io_write_ex(s, v, l, a) if (!zend_stat_io_write(s, v, l)) a
#define zend_stat_io_write_string_ex(s, v, a) if (!zend_stat_io_write_string(s, v)) a
#define zend_stat_io_write_int_ex(s, i, a) if (!zend_stat_io_write_int(s, i)) a
#define zend_stat_io_write_double_ex(s, d, a) if (!zend_stat_io_write_double(s, d)) a
#define zend_stat_io_write_literal_ex(s, v, a) if (!zend_stat_io_write(s, v, sizeof(v)-1)) a

zend_bool zend_stat_io_startup(zend_stat_io_t *io, char *uri, zend_stat_buffer_t *buffer, zend_stat_io_routine_t *routine);
zend_bool zend_stat_io_closed(zend_stat_io_t *io);
void zend_stat_io_shutdown(zend_stat_io_t *io);
#endif	/* ZEND_STAT_IO_H */
