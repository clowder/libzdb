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

static ConnectionPool_T *get_pool_pointer(VALUE self)
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
  ConnectionPool_start(*pool);

  URL_free(&url);

  return self;
}

/*
 * [Libzdb::ConnectionPool#execute] Executes the given sql.
 *
 * Returns the number of rows changed
 */
static VALUE execute(VALUE self, VALUE rb_string)
{
  Check_Type(rb_string, T_STRING);

  char *sql               = StringValueCStr(rb_string);
  VALUE results           = rb_ary_new();
  ConnectionPool_T *pool  = get_pool_pointer(self);
  Connection_T connection = ConnectionPool_getConnection(*pool);

  Connection_execute(connection, sql);

  return INT2NUM(Connection_rowsChanged(connection));
}

/*
 * [Libzdb::ConnectionPool#execute_query] Executes the given sql.
 *
 * Returns an array of hashes
 */
static VALUE execute_query(VALUE self, VALUE rb_string)
{
  Check_Type(rb_string, T_STRING);

  char *sql               = StringValueCStr(rb_string);
  VALUE results           = rb_ary_new();
  ConnectionPool_T *pool  = get_pool_pointer(self);
  Connection_T connection = ConnectionPool_getConnection(*pool);
  ResultSet_T result_set  = Connection_executeQuery(connection, sql);
  int column_count        = ResultSet_getColumnCount(result_set);
  int index;

  while (ResultSet_next(result_set))
  {
    VALUE row_hash = rb_hash_new();

    for(index=column_count; index>0; index--)
    {
      char *column_name  = ResultSet_getColumnName(result_set, index);
      char *column_value = ResultSet_getString(result_set, index);

      rb_hash_aset(row_hash, rb_str_new2(column_name), rb_str_new2(column_value));
    }

    rb_ary_push(results, row_hash);
  }

  return results;
}


VALUE cLibzdbConnectionPool;
void init_connection_pool()
{
  VALUE libzdb = rb_define_module("Libzdb");
  VALUE klass  = rb_define_class_under(libzdb, "ConnectionPool", rb_cObject);

  cLibzdbConnectionPool = klass;

  rb_define_alloc_func(cLibzdbConnectionPool, allocate);
  rb_define_method(cLibzdbConnectionPool, "initialize", initialize, 1);
  rb_define_method(cLibzdbConnectionPool, "execute", execute, 1);
  rb_define_method(cLibzdbConnectionPool, "execute_query", execute_query, 1);
}
