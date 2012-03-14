require 'spec_helper'

describe ZDB do
  describe ".setup" do
    it "creates a connection pool" do
      url = Pathname.new(File.dirname(__FILE__)).join('../tmp/test.db')

      ZDB.setup("sqlite://#{ url.to_s }")
      ZDB.connection_pool.should be_instance_of(ZDB::ConnectionPool)
    end
  end
end

