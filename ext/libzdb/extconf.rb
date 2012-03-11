require 'mkmf'

LIBDIR      = Config::CONFIG['libdir']
INCLUDEDIR  = Config::CONFIG['includedir']

ZDB_HEADER_DIRS = [
  '/opt/local/include/zdb',
  '/usr/local/include/zdb',
  File.join(INCLUDEDIR, 'zdb'),
  '/usr/include/zdb',
]

dir_config('libzdb', ZDB_HEADER_DIRS, LIBDIR)

have_library('zdb')
create_makefile('libzdb')

