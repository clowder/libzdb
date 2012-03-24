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

static VALUE initialize(VALUE self, VALUE rb_string)
{
  Check_Type(rb_string, T_STRING);

  ConnectionPool_T *pool = get_pool_pointer(self);
  char *url_string       = StringValueCStr(rb_string);
  URL_T url              = URL_new(url_string);

  *pool = ConnectionPool_new(url);
  ConnectionPool_setReaper(*pool, 60);
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
static VALUE get_connection(VALUE self)
{
  VALUE args[1];
  args[0] = self;

  return rb_class_new_instance(1, args, cZDBConnection);
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

static void set_initial_connections(VALUE self, VALUE rb_int)
{
  ConnectionPool_T *pool  = get_pool_pointer(self);
  int initial_connections = NUM2INT(rb_int);

  ConnectionPool_setInitialConnections(*pool, initial_connections);
}

static void set_max_connections(VALUE self, VALUE rb_int)
{
  ConnectionPool_T *pool = get_pool_pointer(self);
  int max_connections = NUM2INT(rb_int);

  ConnectionPool_setMaxConnections(*pool, max_connections);
}

VALUE cZDBConnectionPool;
void init_connection_pool()
{
  VALUE zdb   = rb_define_module("ZDB");
  VALUE klass = rb_define_class_under(zdb, "ConnectionPool", rb_cObject);

  cZDBConnectionPool = klass;

  rb_define_alloc_func(cZDBConnectionPool, allocate);
  rb_define_method(cZDBConnectionPool, "initialize", initialize, 1);
  rb_define_method(cZDBConnectionPool, "active", active, 0);
  rb_define_method(cZDBConnectionPool, "size", size, 0);
  rb_define_method(cZDBConnectionPool, "url", url, 0);
  rb_define_method(cZDBConnectionPool, "get_connection", get_connection, 0);
  rb_define_method(cZDBConnectionPool, "initial_connections", initial_connections, 0);
  rb_define_method(cZDBConnectionPool, "max_connections", max_connections, 0);
  rb_define_method(cZDBConnectionPool, "initial_connections=", set_initial_connections, 1);
  rb_define_method(cZDBConnectionPool, "max_connections=", set_max_connections, 1);
}
