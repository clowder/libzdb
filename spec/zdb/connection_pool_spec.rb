require 'spec_helper'

describe ZDB::ConnectionPool do
  subject { ZDB::ConnectionPool.new(uri) }

  let(:uri) { "sqlite://#{ tmp_db_path.to_s }" }

  describe ".new" do
    context "no args" do
      it "configures the instance properly" do
        pool = ZDB::ConnectionPool.new(uri)

        tmp_db_path.should be_file

        pool.url.should == uri
        pool.initial_connections.should == 5
        pool.max_connections.should == 20
      end
    end

    context "with args" do
      let(:args) { Hash[:initial_connections => 2, :max_connections => 10] }

      it "configures the instance properly" do
        pool = ZDB::ConnectionPool.new(uri, args)

        tmp_db_path.should be_file

        pool.initial_connections.should == 2
        pool.max_connections.should == 10
        pool.url.should == uri
      end
    end
  end

  describe "#size" do
    it "returns the number count of all connections in the pool" do
      subject.size.should == 5 # brand new object so === initial_connections
    end
  end

  describe "#active" do
    it "returns the number of _active_ connections in the pool" do
      subject.active.should == 0
    end
  end

  describe "#get_connection" do
    let(:args) { Hash[:max_rows => 100, :query_timeout => 10] }

    context "no args" do
      it "returns a new connection that belongs to the current pool" do
        connection = subject.get_connection

        connection.url.should == subject.url
        connection.connection_pool.should == subject
      end
    end

    context "with args" do
      it "returns a properly configured connection in the current pool" do
        connection = subject.get_connection(args)

        connection.connection_pool.should == subject
        connection.max_rows.should == 100
        connection.query_timeout == 10
      end
    end
  end
end
