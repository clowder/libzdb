# Libzdb

Ruby bindings to libzdb, "A small, easy to use Open Source Database Connection Pool Library"

## Installation

Add this line to your application's Gemfile:

    gem 'libzdb'

And then execute:

    $ bundle

Or install it yourself as:

    $ gem install libzdb

## Usage

```ruby
connection_pool = ZDB::ConnectionPool.new('sqlite:///Users/chris/test.db')
connection      = connection_pool.get_connection

rows_changed = connection.execute(<<-EOSQL)
  CREATE TABLE cars (id INTEGER PRIMARY KEY, model TEXT, brand_id INTEGER, manufactured_at DATETIME);
  CREATE TABLE brands (id INTEGER PRIMARY KEY, name TEXT);
  INSERT INTO brands (name) VALUES ("Ford");
  INSERT INTO brands (name) VALUES ("VW");
  INSERT INTO cars (model, brand_id) VALUES ("Focus", (SELECT id FROM brands WHERE name = "Ford"));
  INSERT INTO cars (model, brand_id) VALUES ("Golf", (SELECT id FROM brands WHERE name = "VW"));
  UPDATE cars SET manufactured_at = CURRENT_TIMESTAMP;
EOSQL

puts rows_changed #=> 2

cars = connection.execute_query(<<-EOSQL)
  SELECT model FROM cars INNER JOIN brands on brands.id = cars.brand_id WHERE brands.name = "VW";
EOSQL

puts cars #=> [{ "model"=>"Golf" }]
```

## Contributing

1. Fork it
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Added some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create new Pull Request
