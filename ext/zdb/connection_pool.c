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
  ConnectionPool_setReaper(*pool, 60);
  ConnectionPool_start(*pool);

  return self;
}

/*
 * [ZDB::ConnectionPool#execute] Executes the given sql.
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
 * [ZDB::ConnectionPool#execute_query] Executes the given sql.
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

VALUE cZDBConnectionPool;
void init_connection_pool()
{
  VALUE zdb   = rb_define_module("ZDB");
  VALUE klass = rb_define_class_under(zdb, "ConnectionPool", rb_cObject);

  cZDBConnectionPool = klass;

  rb_define_alloc_func(cZDBConnectionPool, allocate);
  rb_define_method(cZDBConnectionPool, "initialize", initialize, 1);
  rb_define_method(cZDBConnectionPool, "execute", execute, 1);
  rb_define_method(cZDBConnectionPool, "execute_query", execute_query, 1);
  rb_define_method(cZDBConnectionPool, "active", active, 0);
  rb_define_method(cZDBConnectionPool, "size", size, 0);
  rb_define_method(cZDBConnectionPool, "url", url, 0);
}
