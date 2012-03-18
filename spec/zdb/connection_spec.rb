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
    let(:sql) { %Q{SELECT model FROM cars INNER JOIN brands on brands.id = cars.brand_id WHERE brands.name = "VW";} }

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

  describe "#query_timeout" do
    it "returns the query timeout in milliseconds" do
      subject.query_timeout.should == 3000
    end
  end

  describe "#max_rows" do
    it "returns the maximum number of rows that any query can return" do
      subject.max_rows.should == 0
    end
  end

  describe "#max_rows=" do
    it "sets the maximum number of rows, 0 for no limit" do
      subject.max_rows = 100
      subject.max_rows.should == 100
    end

    it "type checks the incoming value" do
      expect {
        subject.max_rows = "foo"
      }.to raise_error(TypeError)
    end
  end

  describe "#query_timeout=" do
    it "sets the query timeout, 0 for no limit" do
      subject.query_timeout = 0
      subject.query_timeout.should == 0
    end

    it "type checks the incoming value" do
      expect {
        subject.query_timeout = "foo"
      }.to raise_error(TypeError)
    end
  end

  describe "#ping" do
    context "the connection is alive" do
      it "is true" do
        subject.ping.should be_true
      end
    end

    context "the connection is **dead** (dum dum dum *dramtic reverb*)" do
      it "is false" do
        pending("Need a resonable testing strategy")
      end
    end
  end

  describe "#transaction" do
    let(:second_conneciton) { connection_pool.get_connection }
    let(:expected_tables)   { [{ "name" => "brands" }, { "name" => "cars" }] }

    RSpec::Matchers.define :have_tables do |tables|
      match do |connection|
        sql = "SELECT name FROM sqlite_master WHERE type='table' ORDER BY name;"
        tables == connection.execute_query(sql)
      end
    end

    context "when not passed a block" do
      it "explodes" do
        expect {
          subject.transaction
        }.to raise_error(LocalJumpError, "no block given")
      end
    end

    context "when passed a block" do
      it "commits the transaction at the end of the block" do
        subject.transaction do
          subject.execute(schema)

          subject.should have_tables(expected_tables) # We should be able to query inside the transaction
          second_conneciton.should_not have_tables # From another connection we shouldn't see anything
        end

        second_conneciton.should have_tables(expected_tables) # Now we can query the commited data from all connections
      end
    end

    context "rolling back" do
      context "purposefully" do
        it "does what you think it would" do
          subject.transaction do
            subject.execute(schema)
            raise ZDB::Rollback
          end

          second_conneciton.should_not have_tables
        end

        it "doesn't propogate the error" do
          expect {
            subject.transaction do
              subject.execute(schema)
              raise ZDB::Rollback
            end
          }.to_not raise_error
        end
      end

      context "by accident" do
        it "rolls back the transaction and propogates the error" do
          expect {
            subject.transaction do
              subject.execute(schema)
              fail "This was an accident!"
            end
          }.to raise_error(RuntimeError, "This was an accident!")

          second_conneciton.should_not have_tables
        end
      end
    end
  end
end
