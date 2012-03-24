# libzdb (http://github.com/clowder/libzdb)
# Ruby bindings to libzdb, "A small, easy to use Open Source Database Connection Pool Library"
#
# Copyright (c) 2012, Chris Lowder (http://github.com/clowder)
# Released under the MIT License, please see LICENSE

require 'mkmf'

LIBDIR          = Config::CONFIG['libdir']
INCLUDEDIR      = Config::CONFIG['includedir']
ZDB_HEADER_DIRS = ['/opt/local/include/zdb',
                   '/usr/local/include/zdb',
                   File.join(INCLUDEDIR, 'zdb'),
                   '/usr/include/zdb']

dir_config('zdb', ZDB_HEADER_DIRS, LIBDIR)
have_library('zdb')
create_makefile('zdb')
