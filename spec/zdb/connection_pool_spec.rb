require 'spec_helper'

describe ZDB::ConnectionPool do
  subject { ZDB::ConnectionPool.new("sqlite://#{ tmp_db_path.to_s }") }

  describe ".new" do
    it "works" do
      connection_pool = ZDB::ConnectionPool.new("sqlite://#{ tmp_db_path.to_s }")
      tmp_db_path.should be_file
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

  describe "#get_connection" do
    it "returns a new connection that belongs to the current pool" do
      connection = subject.get_connection

      connection.url.should == subject.url
      connection.connection_pool.should == subject
    end
  end

  describe "#initial_connections" do
    it "returns the number of connection that the pool is started with" do
      subject.initial_connections.should == 5
    end
  end

  describe "#max_connections" do
    it "returns the maximum number of connections that can exist in a pool" do
      subject.max_connections.should == 20
    end
  end

  describe "#initial_connections=" do
    it "sets the number of connections that the pool is started with" do
      subject.initial_connections = 2
      subject.initial_connections.should == 2
    end
  end

  describe "#max_connections=" do
    it "sets the maximum number of connections for the pool" do
      subject.max_connections = 10
      subject.max_connections.should == 10
    end
  end
end
