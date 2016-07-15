
CXXFLAGS += -O2 -std=c++11

.PHONY: all

all: test_cow_read test_cow_write test_vec_read test_vec_write test_copy_read test_no_alloc_copy_read

test_cow_read: test_cow_read.cpp cow.h
	$(CXX) $(CXXFLAGS) -o $@ $<

test_cow_write: test_cow_write.cpp cow.h
	$(CXX) $(CXXFLAGS) -o $@ $<

test_vec_read: test_vec_read.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

test_vec_write: test_vec_write.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

test_copy_read: test_copy_read.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

test_no_alloc_copy_read: test_no_alloc_copy_read.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<
