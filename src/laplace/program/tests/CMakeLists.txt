#
# TO keep the tests organized once you start writing real
# ones, I strongly suggest that you make the tests clearly
# correspond to a specific source file, and let the test
# file here have an identical name to the file it is testing.
#
# You can group things by adding more source files to a
# single unit test binary (just add them all at the end
# of add_unit_test() statements), and you can of course also
# add more unit test binaries by having several such statements.
#

add_unit_test(ProgramTests program-test
              testexample.cpp)
              
