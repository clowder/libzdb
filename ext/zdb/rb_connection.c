/*
 * libzdb (http://github.com/clowder/libzdb)
 * Ruby bindings to libzdb, "A small, easy to use Open Source Database Connection Pool Library"
 *
 * Copyright (c) 2012, Chris Lowder (http://github.com/clowder)
 * Released under the MIT License, please see LICENSE
 */

#include <rb_connection.h>

static VALUE allocate(VALUE klass)
{
  Connection_T *connection;
  return Data_Make_Struct(klass, Connection_T, NULL, NULL, connection);
}

static Connection_T *get_connection_pointer(VALUE self)
{
  Connection_T *connection;
  Data_Get_Struct(self, Connection_T, connection);
  return connection;
}

/*
 * Returns a new connection in the given pool.
 *
 * @param [ZDB::ConnectionPool] connection_pool the parent pool for the connection
 * @return [ZDB::Connection]
 */
static VALUE initialize(VALUE self, VALUE connection_pool)
{
  ConnectionPool_T *pool   = get_pool_pointer(connection_pool);
  Connection_T *connection = get_connection_pointer(self);

  *connection = ConnectionPool_getConnection(*pool);

  rb_iv_set(self, "@connection_pool", connection_pool);

  return self;
}

/*
 * Execute the sql on the connection.
 *
 * @param [String] rb_string the SQL to be executed
 * @return [Fixnum] the number for rows changed by the execute
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
 * Query the database with the given sql.
 *
 * @param [String] rb_string the SQL to use for the query
 * @return [Array<Hash>] an array of hashes that contain the query results
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

/*
 * The url for the current connection.
 *
 * @return [String]
 */
static VALUE url(VALUE self)
{
  Connection_T *connection = get_connection_pointer(self);
  URL_T url                = Connection_getURL(*connection);
  char *url_string         = URL_toString(url);

  return rb_str_new2(url_string);
}

/*
 * The parent pool object.
 *
 * @return [ZDB::ConnectionPool]
 */
static VALUE connection_pool(VALUE self)
{
  return rb_iv_get(self, "@connection_pool");
}

/*
 * The length of time (in millieseconds) to wait for a query to finish running.
 *
 * @return [Fixnum] the timeout in milliseconds
 */
static VALUE query_timeout(VALUE self)
{
  Connection_T *connection = get_connection_pointer(self);
  int timeout              = Connection_getQueryTimeout(*connection);

  return INT2NUM(timeout);
}

/*
 * The maximum number of rows a query can return.
 *
 * @return [Fixnum]
 */
static VALUE max_rows(VALUE self)
{
  Connection_T *connection = get_connection_pointer(self);
  int max                  = Connection_getMaxRows(*connection);

  return INT2NUM(max);
}

/*
 * The maximum number of rows a query can return.
 *
 * @param [Fixnum] rb_int the new max
 */
static void set_max_rows(VALUE self, VALUE rb_int)
{
  int max                  = NUM2INT(rb_int);
  Connection_T *connection = get_connection_pointer(self);

  Connection_setMaxRows(*connection, max);
}

/*
 * The length of time (in millieseconds) to wait for a query to finish running.
 *
 * @param [Fixnum] rb_int the new timeout in milliseconds
 */
static void set_query_timeout(VALUE self, VALUE rb_int)
{
  int timeout              = NUM2INT(rb_int);
  Connection_T *connection = get_connection_pointer(self);

  Connection_setQueryTimeout(*connection, timeout);
}

/*
 * Ping the connection to see if its still alive.
 *
 * @return [true, false]
 */
static VALUE ping(VALUE self)
{
  Connection_T *connection = get_connection_pointer(self);
  int ping_result          = Connection_ping(*connection);

  if (ping_result == 1) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

static VALUE transaction_inner(VALUE self)
{
  Connection_T *connection = get_connection_pointer(self);
  Connection_beginTransaction(*connection);

  return rb_yield(Qnil);
}

static VALUE transaction_handler(VALUE self, VALUE exception)
{
  Connection_T *connection = get_connection_pointer(self);

  Connection_rollback(*connection);

  if (!rb_obj_is_kind_of(exception, eZDBRollback)) {
    rb_exc_raise(exception);
  }

  return exception;
}

/*
 * Execute the give block within an SQL transaction.
 *
 * Any [#execute] or [#execute_query] calls to this [ZDB#Connection] instance
 * within the block will be executed within an SQL transaction, that will be
 * commited at the end of the block.
 *
 * At anytime you can raise [ZDB::Rollback] to force the transaction to stop
 * and rollback the transaction. It is important to not that [ZDB::Rollback]
 * exceptions will be "eaten" by the transaction loop, and will not bubble up
 * throught the application.
 *
 * Raising any other exception class will also cause the transaction to
 * rollback. But, unlike [ZDB::Rollback], all other exception types will bubble
 * up to to your application (i.e. not "eaten" by the transaction.
 *
 * @example Update a set of records within a transaction
 *   connection = ZDB::ConnectionPool.new('sqlite:///tmp/test.db').get_connection
 *   connection.transaction do
 *     # Transfer between users
 *     connection.execute(<<-EOSQL)
 *       UPDATE user
 *       SET balance=((SELECT balance FROM user WHERE user.name = 'clowder')-100)
 *       WHERE user.name = "clowder";"
 *     EOSQL
 *
 *     connection.execute(<<-EOSQL)
 *       UPDATE user
 *       SET balance=((SELECT balance FROM user WHERE user.name = 'lsharp')+100)
 *       WHERE user.name = "lsharp";"
 *     EOSQL
 *   end
 *
 */
static void transaction(VALUE self)
{
  rb_need_block();
  Connection_T *connection = get_connection_pointer(self);
  VALUE result             = rb_rescue(transaction_inner, self, transaction_handler, self);

  if (!rb_obj_is_kind_of(result, eZDBRollback)) {
    Connection_commit(*connection);
  }
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
  rb_define_method(cZDBConnection, "connection_pool", connection_pool, 0);
  rb_define_method(cZDBConnection, "query_timeout", query_timeout, 0);
  rb_define_method(cZDBConnection, "max_rows", max_rows, 0);
  rb_define_method(cZDBConnection, "max_rows=", set_max_rows, 1);
  rb_define_method(cZDBConnection, "query_timeout=", set_query_timeout, 1);
  rb_define_method(cZDBConnection, "ping", ping, 0);
  rb_define_method(cZDBConnection, "transaction", transaction, 0);
}
