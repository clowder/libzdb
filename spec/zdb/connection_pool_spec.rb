require 'spec_helper'

describe ZDB::ConnectionPool do
  subject { ZDB::ConnectionPool.new("sqlite://#{ tmp_db_path.to_s }") }

  let(:schema) { <<-EOSQL }
    CREATE TABLE cars (id INTEGER PRIMARY KEY, model TEXT, brand_id INTEGER, manufactured_at DATETIME);
    CREATE TABLE brands (id INTEGER PRIMARY KEY, name TEXT);
    INSERT INTO brands (name) VALUES ("Ford");
    INSERT INTO brands (name) VALUES ("VW");
    INSERT INTO cars (model, brand_id) VALUES ("Focus", (SELECT id FROM brands WHERE name = "Ford"));
    INSERT INTO cars (model, brand_id) VALUES ("Golf", (SELECT id FROM brands WHERE name = "VW"));
    UPDATE cars SET manufactured_at = CURRENT_TIMESTAMP;
  EOSQL

  describe ".new" do
    it "works" do
      connection_pool = ZDB::ConnectionPool.new("sqlite://#{ tmp_db_path.to_s }")
      tmp_db_path.should be_file
    end
  end

  describe "#execute" do

    it "returns the number of rows changed" do
      result = subject.execute(schema)
      result.should == 2
    end
  end

  describe "#execute_query" do
    let(:sql) { %Q{SELECT model FROM cars WHERE model = "Golf";} }

    it "returns an array of hash objects" do
      subject.execute(schema)
      result = subject.execute_query(sql)

      result.should be_instance_of(Array)
      result.count.should == 1
      result.first.should == { 'model' => 'Golf' }
    end
  end

  describe "#size" do
    it "returns the number count of all connections in the pool" do
      subject.size.should == 5
    end
  end

  describe "#active" do
    it "returns the number of _active_ connections in the pool" do
      subject.active.should == 0
    end
  end

  describe "#url" do
    it "returns the db url that the connection pool was configured with" do
      subject.url.should == "sqlite://#{ tmp_db_path.to_s }"
    end
  end
end
