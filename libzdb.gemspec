# -*- encoding: utf-8 -*-
require File.expand_path('../lib/libzdb/version', __FILE__)

Gem::Specification.new do |gem|
  gem.authors       = ['Chris Lowder']
  gem.email         = ['clowder@gmail.com']
  gem.description   = 'A ruby interface to libzdb.'
  gem.summary       = 'A ruby interface to libzdb'
  gem.homepage      = 'https://github.com/clowder/libzdb'

  gem.add_development_dependency('rake-compiler')
  gem.add_development_dependency('rspec')
  gem.add_development_dependency('ZenTest')
  gem.add_development_dependency('yard')

  gem.extensions    = %w{ ext/zdb/extconf.rb }
  gem.files         = %w{ LICENSE
                          README.md
                          ext/zdb/extconf.rb
                          ext/zdb/rb_connection.c
                          ext/zdb/rb_connection.h
                          ext/zdb/rb_connection_pool.c
                          ext/zdb/rb_connection_pool.h
                          ext/zdb/zdb.c
                          ext/zdb/zdb.h
                          lib/libzdb.rb
                          lib/libzdb/version.rb }
  gem.test_files    = %w{ spec/spec_helper.rb
                          spec/zdb/connection_pool_spec.rb
                          spec/zdb/connection_spec.rb }
  gem.name          = 'libzdb'
  gem.require_paths = ['lib']
  gem.version       = Libzdb::VERSION
end
