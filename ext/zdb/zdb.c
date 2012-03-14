#include <zdb.h>

/*
 * [ZDB.setup] Configures a default [ZDB::ConnectionPool]
 */
static void setup(VALUE self, VALUE url)
{
  VALUE args[1], pool;

  args[0] = url;
  pool    = rb_class_new_instance(1, args, cZDBConnectionPool);

  rb_iv_set(self, "@connection_pool", pool);
}

/*
 * [ZDB.connection_pool] Returns the default [ZDB::ConnectionPool]
 */
static VALUE connection_pool(VALUE self)
{
  return rb_iv_get(self, "@connection_pool");
}

VALUE mZDB;
void Init_zdb()
{
  mZDB = rb_define_module("ZDB");

  rb_define_singleton_method(mZDB, "setup", setup, 1);
  rb_define_singleton_method(mZDB, "connection_pool", connection_pool, 0);

  init_connection_pool();
}

