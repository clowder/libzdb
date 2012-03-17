require 'spec_helper'

describe ZDB::ConnectionPool do
  subject { ZDB::Connection.new(connection_pool) }

  let(:connection_pool) { ZDB::ConnectionPool.new("sqlite://#{ tmp_db_path.to_s }") }
  let(:schema)          { <<-EOSQL }
    CREATE TABLE cars (id INTEGER PRIMARY KEY, model TEXT, brand_id INTEGER, manufactured_at DATETIME);
    CREATE TABLE brands (id INTEGER PRIMARY KEY, name TEXT);
    INSERT INTO brands (name) VALUES ("Ford");
    INSERT INTO brands (name) VALUES ("VW");
    INSERT INTO cars (model, brand_id) VALUES ("Focus", (SELECT id FROM brands WHERE name = "Ford"));
    INSERT INTO cars (model, brand_id) VALUES ("Golf", (SELECT id FROM brands WHERE name = "VW"));
    UPDATE cars SET manufactured_at = CURRENT_TIMESTAMP;
  EOSQL

  describe ".new" do
    it "returns a connection in the given connection pool" do
      connection = ZDB::Connection.new(connection_pool)
      connection.url.should == connection_pool.url
    end

    context "invalid initialization" do
      it "it blows if there is no connection pool" do
        expect {
          ZDB::Connection.new
        }.to raise_error(ArgumentError)
      end

      it "blows if the incoming type is wrong" do
        expect {
          ZDB::Connection.new("sqlite://")
        }.to raise_error(TypeError)
      end
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

  describe "#url" do
    it "returns the url the connection was started on" do
      subject.url.should == "sqlite://#{ tmp_db_path.to_s }"
    end
  end

  describe "#connection_pool" do
    it "returns the pool that the connection was initialized with" do
      subject.connection_pool.should == connection_pool
    end
  end
end
