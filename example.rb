
require 'rsigsegv'

class A
  def foo
    SigSegv.trigger
  end
  
  def bar
    foo
  end
  
  def baz
    bar
  end
end

SigSegv.install_handler

A.new.baz


puts "life goes on..."
