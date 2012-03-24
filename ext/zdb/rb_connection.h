/*
 * libzdb (http://github.com/clowder/libzdb)
 * Ruby bindings to libzdb, "A small, easy to use Open Source Database Connection Pool Library"
 *
 * Copyright (c) 2012, Chris Lowder (http://github.com/clowder)
 * Released under the MIT License, please see LICENSE
 */

#ifndef ZDB_CONNECTION
#define ZDB_CONNECTION

#include <zdb.h>
#include <rb_connection_pool.h>

void init_connection();

extern VALUE cZDBConnection;

#endif

