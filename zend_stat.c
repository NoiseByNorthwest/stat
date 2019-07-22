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

#ifndef ZEND_STAT
# define ZEND_STAT

#define ZEND_STAT_EXTNAME   "Stat"
#define ZEND_STAT_VERSION   "0.0.1-dev"
#define ZEND_STAT_AUTHOR    "krakjoe"
#define ZEND_STAT_URL       "https://github.com/krakjoe/stat"
#define ZEND_STAT_COPYRIGHT "Copyright (c) 2019"

#if defined(__GNUC__) && __GNUC__ >= 4
# define ZEND_STAT_EXTENSION_API __attribute__ ((visibility("default")))
#else
# define ZEND_STAT_EXTENSION_API
#endif

#include "zend_stat.h"
#include "zend_stat_buffer.h"
#include "zend_stat_ini.h"
#include "zend_stat_io.h"
#include "zend_stat_sampler.h"
#include "zend_stat_strings.h"

static zend_stat_buffer_t*     zend_stat_buffer = NULL;
static zend_bool               zend_stat_started = 0;

static int  zend_stat_startup(zend_extension*);
static void zend_stat_shutdown(zend_extension *);
static void zend_stat_activate(void);
static void zend_stat_deactivate(void);

ZEND_STAT_EXTENSION_API zend_extension_version_info extension_version_info = {
    ZEND_EXTENSION_API_NO,
    ZEND_EXTENSION_BUILD_ID
};

ZEND_STAT_EXTENSION_API zend_extension zend_extension_entry = {
    ZEND_STAT_EXTNAME,
    ZEND_STAT_VERSION,
    ZEND_STAT_AUTHOR,
    ZEND_STAT_URL,
    ZEND_STAT_COPYRIGHT,
    zend_stat_startup,
    zend_stat_shutdown,
    zend_stat_activate,
    zend_stat_deactivate,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    STANDARD_ZEND_EXTENSION_PROPERTIES
};

static int zend_stat_startup(zend_extension *ze) {
    zend_stat_ini_startup();

    if (!zend_stat_ini_socket && !zend_stat_ini_dump) {
        zend_error(E_WARNING,
            "[STAT] socket and dump are both disabled by configuration, "
            "may be misconfigured");
        zend_stat_ini_shutdown();

        return SUCCESS;
    }

    if (!zend_stat_strings_startup(zend_stat_ini_strings)) {
        zend_stat_ini_shutdown();

        return SUCCESS;
    }

    if (!(zend_stat_buffer = zend_stat_buffer_startup(zend_stat_ini_slots, zend_stat_ini_interval))) {
        zend_stat_strings_shutdown();
        zend_stat_ini_shutdown();

        return SUCCESS;
    }

    if (!zend_stat_io_startup(zend_stat_ini_socket, zend_stat_buffer)) {
        zend_stat_buffer_shutdown(zend_stat_buffer);
        zend_stat_strings_shutdown();
        zend_stat_ini_shutdown();

        return SUCCESS;
    }

    zend_stat_started  = 1;

    ze->handle = 0;

    return SUCCESS;
}

static void zend_stat_shutdown(zend_extension *ze) {
    if (!zend_stat_started) {
        return;
    }

    if (zend_stat_ini_dump > 0) {
        zend_stat_buffer_dump(zend_stat_buffer, zend_stat_ini_dump);
    }

    zend_stat_io_shutdown();
    zend_stat_buffer_shutdown(zend_stat_buffer);
    zend_stat_ini_shutdown();

    zend_stat_started = 0;
}

static void zend_stat_activate(void) {
#if defined(ZTS) && defined(COMPILE_DL_STAT)
    ZEND_TSRMLS_CACHE_UPDATE();
#endif

    if (!zend_stat_started) {
        return;
    }

#if defined(ZTS)
    zend_stat_sampler_activate(zend_stat_buffer, syscall(SYS_gettid));
#else
    zend_stat_sampler_activate(zend_stat_buffer, getpid());
#endif
}

static void zend_stat_deactivate(void) {
#if defined(ZTS)
    zend_stat_sampler_deactivate(zend_stat_buffer, syscall(SYS_gettid));
#else
    zend_stat_sampler_deactivate(zend_stat_buffer, getpid());
#endif
}

#if defined(ZTS) && defined(COMPILE_DL_STAT)
    ZEND_TSRMLS_CACHE_DEFINE();
#endif

#endif /* ZEND_STAT */