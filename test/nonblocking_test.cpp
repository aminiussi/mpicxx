// Copyright (C) 2006 Douglas Gregor.

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// A test of the nonblocking point-to-point operations.
#include <boost/mpicxx/nonblocking.hpp>
#include <boost/mpicxx/communicator.hpp>
#include <boost/mpicxx/environment.hpp>
#include "check_test.hpp"
#include "gps_position.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/list.hpp>
#include <iterator>
#include <algorithm>

using boost::mpicxx::communicator;
using boost::mpicxx::request;
using boost::mpicxx::status;

enum method_kind { 
  mk_wait_any, mk_test_any, mk_wait_all, mk_wait_all_keep, 
  mk_test_all, mk_test_all_keep, mk_wait_some, mk_wait_some_keep,
  mk_test_some, mk_test_some_keep,
  mk_test_size
};

static const char* method_kind_names[mk_test_size] = {
  "wait_any",
  "test_any",
  "wait_all",
  "wait_all (keep results)",
  "test_all",
  "test_all (keep results)",
  "wait_some",
  "wait_some (keep results)",
  "test_some",
  "test_some (keep results)"
};


template<typename T>
void
nonblocking_tests( const communicator& comm, const T* values, int num_values, 
                   const char* kind, bool composite)
{
  nonblocking_test(comm, values, num_values, kind, mk_wait_any);
  nonblocking_test(comm, values, num_values, kind, mk_test_any);
  nonblocking_test(comm, values, num_values, kind, mk_wait_all);
  nonblocking_test(comm, values, num_values, kind, mk_wait_all_keep);
  if (!composite) {
    nonblocking_test(comm, values, num_values, kind, mk_test_all);
    nonblocking_test(comm, values, num_values, kind, mk_test_all_keep);
  }
  nonblocking_test(comm, values, num_values, kind, mk_wait_some);
  nonblocking_test(comm, values, num_values, kind, mk_wait_some_keep);
  nonblocking_test(comm, values, num_values, kind, mk_test_some);
  nonblocking_test(comm, values, num_values, kind, mk_test_some_keep);
}

template<typename T>
void
nonblocking_test(const communicator& comm, const T* values, int num_values, 
                 const char* kind, method_kind method)
{
  using boost::mpicxx::wait_any;
  using boost::mpicxx::test_any;
  using boost::mpicxx::wait_all;
  using boost::mpicxx::test_all;
  using boost::mpicxx::wait_some;
  using boost::mpicxx::test_some;

  if (comm.rank() == 0) {
    std::cout << "Testing " << method_kind_names[method] 
              << " with " << kind << "...";
    std::cout.flush();
  }
  
  typedef std::pair<status, std::vector<request>::iterator> 
    status_iterator_pair;
  
  T incoming_value;
  std::vector<T> incoming_values(num_values);
  
  std::vector<request> reqs;
  // Send/receive the first value
  reqs.push_back(comm.isend((comm.rank() + 1) % comm.size(), 0, values[0]));
  reqs.push_back(comm.irecv((comm.rank() + comm.size() - 1) % comm.size(),
                            0, incoming_value));
  
  if (method != mk_wait_any && method != mk_test_any) {
#ifndef LAM_MPI
    // We've run into problems here (with 0-length messages) with
    // LAM/MPI on Mac OS X and x86-86 Linux. Will investigate
    // further at a later time, but the problem only seems to occur
    // when using shared memory, not TCP.
    
    // Send/receive an empty message
    reqs.push_back(comm.isend((comm.rank() + 1) % comm.size(), 1));
    reqs.push_back(comm.irecv((comm.rank() + comm.size() - 1) % comm.size(),
                              1));
#endif
    
    // Send/receive an array
    reqs.push_back(comm.isend((comm.rank() + 1) % comm.size(), 2, values,
                              num_values));
    reqs.push_back(comm.irecv((comm.rank() + comm.size() - 1) % comm.size(),
                              2, &incoming_values.front(), num_values));
  }
  
  switch (method) {
  case mk_wait_any:
    if (wait_any(reqs.begin(), reqs.end()).second == reqs.begin())
      reqs[1].wait();
    else
      reqs[0].wait();
    break;
    
  case mk_test_any:
    {
      boost::optional<status_iterator_pair> result;
      do {
        result = test_any(reqs.begin(), reqs.end());
      } while (!result);
      if (result->second == reqs.begin())
        reqs[1].wait();
      else
        reqs[0].wait();
      break;
    }
    
  case mk_wait_all:
    wait_all(reqs.begin(), reqs.end());
    break;
    
  case mk_wait_all_keep:
    {
      std::vector<status> stats;
      wait_all(reqs.begin(), reqs.end(), std::back_inserter(stats));
    }
    break;
    
  case mk_test_all:
    while (!test_all(reqs.begin(), reqs.end())) { /* Busy wait */ }
    break;
    
  case mk_test_all_keep:
    {
      std::vector<status> stats;
      while (!test_all(reqs.begin(), reqs.end(), std::back_inserter(stats)))
        /* Busy wait */;
    }
    break;
    
  case mk_wait_some:
    {
      std::vector<request>::iterator pos = reqs.end();
      do {
        pos = wait_some(reqs.begin(), pos);
      } while (pos != reqs.begin());
    }
    break;
    
  case mk_wait_some_keep:
    {
      std::vector<status> stats;
      std::vector<request>::iterator pos = reqs.end();
      do {
        pos = wait_some(reqs.begin(), pos, std::back_inserter(stats)).second;
      } while (pos != reqs.begin());
    }
    break;
    
  case mk_test_some:
    {
      std::vector<request>::iterator pos = reqs.end();
      do {
        pos = test_some(reqs.begin(), pos);
      } while (pos != reqs.begin());
    }
    break;
    
  case mk_test_some_keep:
    {
      std::vector<status> stats;
      std::vector<request>::iterator pos = reqs.end();
      do {
        pos = test_some(reqs.begin(), pos, std::back_inserter(stats)).second;
      } while (pos != reqs.begin());
    }
    break;
    
  default:
    check_test(comm, false);
  }
  
  if (comm.rank() == 0) {
    bool okay = true;
    
    if (!((incoming_value == values[0])))
      okay = false;
    
    if (method != mk_wait_any && method != mk_test_any
        && !std::equal(incoming_values.begin(), incoming_values.end(),
                       values))
      okay = false;
    
    if (okay)
      std::cout << "OK." << std::endl;
    else
      std::cerr << "ERROR!" << std::endl;
  }
  
  check_test(comm, incoming_value == values[0]);
  
  if (method != mk_wait_any && method != mk_test_any)
    check_test(comm, std::equal(incoming_values.begin(), incoming_values.end(),
                                values));
}

int main(int argc, char* argv[])
{
  boost::mpicxx::environment env(argc, argv);

  communicator comm;

  int int_array[3] = {17, 42, 256};
  nonblocking_tests(comm, int_array, 3, "integers", false);

  gps_position gps_array[2] = {
    gps_position(17, 42, .06),
    gps_position(42, 17, .06)
  };
  nonblocking_tests(comm, gps_array, 2, "gps positions", false);

  std::string string_array[2] = { "Hello", "World" };
  nonblocking_tests(comm, string_array, 2, "strings", true);

  std::list<std::string> lst_of_strings;
  for (int i = 0; i < comm.size(); ++i)
    lst_of_strings.push_back(boost::lexical_cast<std::string>(i));

  nonblocking_tests(comm, &lst_of_strings, 1, "list of strings", true);

  return 0;
}
