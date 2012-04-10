/*
 * libzdb (http://github.com/clowder/libzdb)
 * Ruby bindings to libzdb, "A small, easy to use Open Source Database Connection Pool Library"
 *
 * Copyright (c) 2012, Chris Lowder (http://github.com/clowder)
 * Released under the MIT License, please see LICENSE
 */

#include <zdb.h>

VALUE mZDB;
VALUE eZDBRollback;

void Init_zdb()
{
  mZDB          = rb_define_module("ZDB");
  eZDBRollback  = rb_define_class_under(mZDB, "Rollback", rb_eStandardError);

  init_connection_pool();
  init_connection();
}

