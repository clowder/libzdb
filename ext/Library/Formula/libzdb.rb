require 'formula'

def disable_mysql?;      ARGV.include? '--disable-mysql';      end
def disable_postgresql?; ARGV.include? '--disable-postgresql'; end
def disable_sqlite?;     ARGV.include? '--disable-sqlite';     end

class Libzdb < Formula
  homepage 'http://tildeslash.com/libzdb/'
  url 'http://tildeslash.com/libzdb/dist/libzdb-2.10.1.tar.gz'
  md5 'f0f1d87241c5605af0bcc6407d134d69'

  depends_on 'postgres' unless disable_postgresql?
  depends_on 'mysql'    unless disable_mysql?
  depends_on 'sqlite'   unless disable_sqlite?

  def install
    system "./configure", "--disable-debug", "--disable-dependency-tracking", "--prefix=#{prefix}"
    system "make install"
  end
end
