require 'libzdb'

RSpec.configure do |config|
  config.before(:each) do
    FileUtils.rm(tmp_db_path) if tmp_db_path.file?
  end
end

def tmp_db_path
  Pathname.new(File.dirname(__FILE__)).join('../tmp/test.db')
end

