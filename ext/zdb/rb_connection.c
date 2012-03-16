#include <rb_connection.h>

static void deallocate(Connection_T *connection)
{
  Connection_close(*connection);
}

static VALUE allocate(VALUE klass)
{
  Connection_T *connection = malloc(sizeof(Connection_T));
  return Data_Wrap_Struct(klass, NULL, deallocate, connection);
}

static Connection_T *get_connection_pointer(VALUE self)
{
  Connection_T *connection;
  Data_Get_Struct(self, Connection_T, connection);
  return connection;
}

/*
 * [ZDB::Connection#new]
 *
 * Requires [ZDB::ConnectionPool]
 *
 * Returns [ZDB::Connection]
 */
static VALUE initialize(VALUE self, VALUE connection_pool)
{
  ConnectionPool_T *pool   = get_pool_pointer(connection_pool);
  Connection_T *connection = get_connection_pointer(self);

  *connection = ConnectionPool_getConnection(*pool);

  return self;
}

/*
 * [ZDB::Connection#execute] Executes the given sql.
 *
 * Returns the number of rows changed
 */
static VALUE execute(VALUE self, VALUE rb_string)
{
  Check_Type(rb_string, T_STRING);

  char *sql                = StringValueCStr(rb_string);
  VALUE results            = rb_ary_new();
  Connection_T *connection = get_connection_pointer(self);

  Connection_execute(*connection, sql);

  return INT2NUM(Connection_rowsChanged(*connection));
}

/*
 * [ZDB::Connection#execute_query] Executes the given sql.
 *
 * Returns an array of hashes
 */
static VALUE execute_query(VALUE self, VALUE rb_string)
{
  Check_Type(rb_string, T_STRING);

  char *sql                = StringValueCStr(rb_string);
  VALUE results            = rb_ary_new();
  Connection_T *connection = get_connection_pointer(self);
  ResultSet_T result_set   = Connection_executeQuery(*connection, sql);
  int column_count         = ResultSet_getColumnCount(result_set);
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

static VALUE url(VALUE self)
{
  Connection_T *connection = get_connection_pointer(self);
  URL_T url                = Connection_getURL(*connection);
  char *url_string         = URL_toString(url);

  return rb_str_new2(url_string);
}

VALUE cZDBConnection;
void init_connection()
{
  VALUE mZDB = rb_define_module("ZDB");

  cZDBConnection = rb_define_class_under(mZDB, "Connection", rb_cObject);

  rb_define_alloc_func(cZDBConnection, allocate);
  rb_define_method(cZDBConnection, "initialize", initialize, 1);
  rb_define_method(cZDBConnection, "execute", execute, 1);
  rb_define_method(cZDBConnection, "execute_query", execute_query, 1);
  rb_define_method(cZDBConnection, "url", url, 0);
}
