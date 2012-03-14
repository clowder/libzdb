#include <libzdb.h>

/*
 * [Libzdb.setup] Configures a default [Libzdb::ConnectionPool]
 */
static void setup(VALUE self, VALUE url)
{
  VALUE args[1], pool;

  args[0] = url;
  pool    = rb_class_new_instance(1, args, cLibzdbConnectionPool);

  rb_iv_set(self, "@connection_pool", pool);
}

/*
 * [Libzdb.connection_pool] Returns the default [Libzdb::ConnectionPool]
 */
static VALUE connection_pool(VALUE self)
{
  return rb_iv_get(self, "@connection_pool");
}

VALUE mLibzdb;
void Init_libzdb()
{
  mLibzdb = rb_define_module("Libzdb");

  rb_define_singleton_method(mLibzdb, "setup", setup, 1);
  rb_define_singleton_method(mLibzdb, "connection_pool", connection_pool, 0);

  init_connection_pool();
}

