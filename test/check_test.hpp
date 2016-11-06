#include <cstdlib>
#include <iostream>
#include <boost/mpicxx/communicator.hpp>

struct nocomm {};

inline 
void
check_test(boost::mpicxx::communicator const& comm, bool cond) {
  if (!cond) {
    std::abort();
  } else {
    if (comm.rank() == 0) {
      std::cout << "OK\n";
    }
  }
}

inline 
void
check_test(nocomm const& comm, bool cond) {
  if (!cond) {
    std::abort();
  } else {
    std::cout << "OK\n";
  }
}
