/*
 * libzdb (http://github.com/clowder/libzdb)
 * Ruby bindings to libzdb, "A small, easy to use Open Source Database Connection Pool Library"
 *
 * Copyright (c) 2012, Chris Lowder (http://github.com/clowder)
 * Released under the MIT License, please see LICENSE
 */

#ifndef ZDB
#define ZDB

#include <ruby.h>

#include <zdb/zdb.h>

#define STRING2SYM(str) ID2SYM(rb_intern(str))

#include <rb_connection_pool.h>
#include <rb_connection.h>

extern VALUE mZDB;
extern VALUE eZDBRollback;

#endif
