# -*- encoding: utf-8 -*-
require File.expand_path('../lib/libzdb/version', __FILE__)

Gem::Specification.new do |gem|
  gem.authors       = ["Chris Lowder"]
  gem.email         = ["clowder@gmail.com"]
  gem.description   = %q{TODO: Write a gem description}
  gem.summary       = %q{TODO: Write a gem summary}
  gem.homepage      = ""

  gem.add_development_dependency('rake-compiler')
  gem.add_development_dependency('rspec')
  gem.add_development_dependency('ZenTest')

  gem.executables   = `git ls-files -- bin/*`.split("\n").map{ |f| File.basename(f) }
  gem.files         = `git ls-files`.split("\n")
  gem.test_files    = `git ls-files -- {test,spec,features}/*`.split("\n")
  gem.name          = "libzdb"
  gem.require_paths = ["lib"]
  gem.version       = Libzdb::VERSION
end
