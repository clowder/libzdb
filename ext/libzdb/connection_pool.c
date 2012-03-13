#include <connection_pool.h>

static void deallocate(ConnectionPool_T *pool)
{
  ConnectionPool_stop(*pool);
  ConnectionPool_free(pool);
}

static VALUE allocate(VALUE klass)
{
  ConnectionPool_T *pool = malloc(sizeof(ConnectionPool_T));
  return Data_Wrap_Struct(klass, NULL, deallocate, pool);
}

static VALUE initialize(VALUE self, VALUE rb_string)
{
  Check_Type(rb_string, T_STRING);

  ConnectionPool_T *pool;
  
  char *url_string = StringValueCStr(rb_string);
  URL_T url        = URL_new(url_string);

  Data_Get_Struct(self, ConnectionPool_T, pool);

  *pool = ConnectionPool_new(url);
  ConnectionPool_start(*pool);

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
