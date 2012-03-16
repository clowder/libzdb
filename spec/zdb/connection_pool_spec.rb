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
end
