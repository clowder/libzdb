#include <zdb.h>

VALUE mZDB;
VALUE eZDBRollback;
void Init_zdb()
{
  mZDB         = rb_define_module("ZDB");
  eZDBRollback = rb_define_class_under(mZDB, "Rollback", rb_eStandardError);

  init_connection_pool();
  init_connection();
}

