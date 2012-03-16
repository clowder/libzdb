#ifndef ZDB_CONNECTION_POOL
#define ZDB_CONNECTION_POOL

#include <zdb.h>

void init_connection_pool();
ConnectionPool_T *get_pool_pointer(VALUE self);

extern VALUE cZDBConnectionPool;

#endif
