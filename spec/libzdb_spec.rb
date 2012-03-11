require 'spec_helper'

describe Libzdb do
  describe ".setup" do
    it "creates a connection pool" do
      url = Pathname.new(File.dirname(__FILE__)).join('../tmp/test.db')

      Libzdb.setup("sqlite://#{ url.to_s }")
      Libzdb.connection_pool.should be_instance_of(Libzdb::ConnectionPool)
    end
  end
end

