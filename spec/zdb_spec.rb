require 'spec_helper'

describe ZDB do
  describe ".setup" do
    it "creates a connection pool" do
      ZDB.setup("sqlite://#{ tmp_db_path.to_s }")
      ZDB.connection_pool.should be_instance_of(ZDB::ConnectionPool)
    end
  end
end

