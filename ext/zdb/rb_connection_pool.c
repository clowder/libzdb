/*
 * libzdb (http://github.com/clowder/libzdb)
 * Ruby bindings to libzdb, "A small, easy to use Open Source Database Connection Pool Library"
 *
 * Copyright (c) 2012, Chris Lowder (http://github.com/clowder)
 * Released under the MIT License, please see LICENSE
 */

#include <rb_connection_pool.h>

static void deallocate(ConnectionPool_T *pool)
{
  ConnectionPool_stop(*pool);
  ConnectionPool_free(pool);
}

static VALUE allocate(VALUE klass)
{
  ConnectionPool_T *pool;
  return Data_Make_Struct(klass, ConnectionPool_T, NULL, deallocate, pool);
}

ConnectionPool_T *get_pool_pointer(VALUE self)
{
  ConnectionPool_T *pool;
  Data_Get_Struct(self, ConnectionPool_T, pool);
  return pool;
}

static void errorHandler(char *newline_terminated_message)
{
  int message_length = strlen(newline_terminated_message) - 1;
  char message[message_length];

  strncpy(&message, newline_terminated_message, message_length);

  rb_raise(eZDBConnectionPool, message);
}

static VALUE initialize(int argc, VALUE *argv, VALUE self)
{
  VALUE rb_url_string, options_hash, rb_initial_connections, rb_max_connections;

  // (int arg count, VALUE* args, char* "(no. required args)(no. optional args)", VALUE* individual args ...
  rb_scan_args(argc, argv, "11", &rb_url_string, &options_hash);

  Check_Type(rb_url_string, T_STRING);

  ConnectionPool_T *pool = get_pool_pointer(self);
  char *url_string       = StringValueCStr(rb_url_string);
  URL_T url              = URL_new(url_string);

  *pool = ConnectionPool_new(url);

  if (!NIL_P(options_hash)) {
    // Configure based on our options hash
    Check_Type(options_hash, T_HASH);

    rb_initial_connections = rb_hash_aref(options_hash, STRING2SYM("initial_connections"));
    rb_max_connections     = rb_hash_aref(options_hash, STRING2SYM("max_connections"));

    if (!NIL_P(rb_initial_connections)) {
      ConnectionPool_setInitialConnections(*pool, NUM2INT(rb_initial_connections));
    }

    if (!NIL_P(rb_max_connections)) {
      ConnectionPool_setMaxConnections(*pool, NUM2INT(rb_max_connections));
    }
  }

  ConnectionPool_setReaper(*pool, 60);
  ConnectionPool_setAbortHandler(*pool, errorHandler);
  ConnectionPool_start(*pool);

  return self;
}

/*
 * [ZDB::ConnectionPool#size] The number of connections (active and inactive)
 * in the connection pool.
 *
 * Returns an Integer
 */
static VALUE size(VALUE self)
{
  ConnectionPool_T *pool = get_pool_pointer(self);
  int pool_size          = ConnectionPool_size(*pool);

  return INT2NUM(pool_size);
}

/*
 * [ZDB::ConnectionPool#active] The number of active connections in the
 * connection pool.
 *
 * Returns an Integer
 */
static VALUE active(VALUE self)
{
  ConnectionPool_T *pool = get_pool_pointer(self);
  int active_in_pool     = ConnectionPool_active(*pool);

  return INT2NUM(active_in_pool);
}

/*
 * [ZDB::ConnectionPool#url] The url the connection pool was configured with
 *
 * Returns a string
 */
static VALUE url(VALUE self)
{
  ConnectionPool_T *pool = get_pool_pointer(self);
  URL_T url              = ConnectionPool_getURL(*pool);
  char *url_string       = URL_toString(url);

  return rb_str_new2(url_string);
}

/*
 * [ZDB::ConnectionPool#get_connection]
 *
 * Returns a connection from the pool
 */
static VALUE get_connection(int argc, VALUE *argv, VALUE self)
{
  VALUE options[argc+1];
  int index;

  options[0] = self;

  for (index = 0; index < argc; index++) {
    options[index+1] = argv[index];
  }

  return rb_class_new_instance(sizeof(options)/SIZEOF_VALUE, options, cZDBConnection);
}

static VALUE initial_connections(VALUE self)
{
  ConnectionPool_T *pool = get_pool_pointer(self);
  return INT2NUM(ConnectionPool_getInitialConnections(*pool));
}

static VALUE max_connections(VALUE self)
{
  ConnectionPool_T *pool = get_pool_pointer(self);
  return INT2NUM(ConnectionPool_getMaxConnections(*pool));
}

VALUE cZDBConnectionPool;
VALUE eZDBConnectionPool;

void init_connection_pool()
{
  VALUE zdb = rb_define_module("ZDB");

  cZDBConnectionPool = rb_define_class_under(zdb, "ConnectionPool", rb_cObject);
  eZDBConnectionPool = rb_define_class_under(cZDBConnectionPool, "Exception", rb_eStandardError);


  rb_define_alloc_func(cZDBConnectionPool, allocate);
  rb_define_method(cZDBConnectionPool, "initialize", initialize, -1);
  rb_define_method(cZDBConnectionPool, "active", active, 0);
  rb_define_method(cZDBConnectionPool, "size", size, 0);
  rb_define_method(cZDBConnectionPool, "url", url, 0);
  rb_define_method(cZDBConnectionPool, "get_connection", get_connection, -1);
  rb_define_method(cZDBConnectionPool, "initial_connections", initial_connections, 0);
  rb_define_method(cZDBConnectionPool, "max_connections", max_connections, 0);
}
