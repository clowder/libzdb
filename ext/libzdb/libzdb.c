#include <ruby.h>

static VALUE hello_world(VALUE mod)
{
  return rb_str_new2("hello world");
}

void Init_libzdb()
{
  VALUE mLibzdb = rb_define_module("Libzdb");
  rb_define_singleton_method(mLibzdb, "hello_world", hello_world, 0);
}

