/*
 * libzdb (http://github.com/clowder/libzdb)
 * Ruby bindings to libzdb, "A small, easy to use Open Source Database Connection Pool Library"
 *
 * Copyright (c) 2012, Chris Lowder (http://github.com/clowder)
 * Released under the MIT License, please see LICENSE
 */

#ifndef ZDB_CONNECTION_POOL
#define ZDB_CONNECTION_POOL

#include <zdb.h>

void init_connection_pool();
ConnectionPool_T *get_pool_pointer(VALUE self);

extern VALUE cZDBConnectionPool;
extern VALUE eZDBConnectionPool;

#endif
