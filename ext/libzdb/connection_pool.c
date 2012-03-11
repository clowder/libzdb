#include <connection_pool.h>

static void deallocate(ConnectionPool_T *pool)
{
  ConnectionPool_stop(*pool);
  ConnectionPool_free(pool);
}

/*
 * Wrap libzdb's connection pool in a ruby object
 */
static VALUE allocate(VALUE klass)
{
  ConnectionPool_T *pool;
  return Data_Wrap_Struct(klass, NULL, deallocate, pool);
}

static VALUE initialize(VALUE self, VALUE rb_string)
{
  Check_Type(rb_string, T_STRING);

  ConnectionPool_T *poolPtr;
  ConnectionPool_T pool;
  URL_T url;
  char *url_string;

  Data_Get_Struct(self, ConnectionPool_T, poolPtr);

  url_string = StringValueCStr(rb_string);
  url        = URL_new(url_string);
  pool       = ConnectionPool_new(url);

  ConnectionPool_start(pool);

  poolPtr = &pool;

  URL_free(&url);

  return self;
}

VALUE cLibzdbConnectionPool;
void init_connection_pool()
{
  VALUE libzdb = rb_define_module("Libzdb");
  VALUE klass  = rb_define_class_under(libzdb, "ConnectionPool", rb_cObject);

  cLibzdbConnectionPool = klass;

  rb_define_alloc_func(cLibzdbConnectionPool, allocate);
  rb_define_method(cLibzdbConnectionPool, "initialize", initialize, 1);
}
