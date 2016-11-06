// Copyright (C) 2013 Alain Miniussi <alain.miniussi@oca.eu>

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// test threading::level operations

#include <boost/mpicxx/environment.hpp>
#include "check_test.hpp"
#include <iostream>
#include <sstream>

namespace mpi = boost::mpicxx;

void
test_threading_level_io(mpi::communicator const& comm,
                        mpi::threading::level orig) {
  std::ostringstream out;
  namespace mt = boost::mpicxx::threading;
  mt::level printed = mt::level(-1);

  out << orig;
  check_test(comm, out.good());
  std::string orig_str(out.str());
  std::cout << "orig string:" << orig_str << '\n';
  std::istringstream in(orig_str);
  in >> printed;
  check_test(comm, !in.bad());
  std::cout << "orig: " << orig << ", printed: " << printed << std::endl;
  check_test(comm, orig == printed);
}

void
test_threading_levels_io(mpi::communicator const& comm) {
  namespace mt = boost::mpicxx::threading;
  test_threading_level_io(comm, mt::single);
  test_threading_level_io(comm, mt::funneled);
  test_threading_level_io(comm, mt::serialized);
  test_threading_level_io(comm, mt::multiple);
}

void
test_threading_level_cmp(mpi::communicator const& comm) {
  namespace mt = boost::mpicxx::threading;
  check_test(comm, mt::single == mt::single);
  check_test(comm, mt::funneled == mt::funneled);
  check_test(comm, mt::serialized == mt::serialized);
  check_test(comm, mt::multiple == mt::multiple);
  
  check_test(comm, mt::single != mt::funneled);
  check_test(comm, mt::single != mt::serialized);
  check_test(comm, mt::single != mt::multiple);

  check_test(comm, mt::funneled != mt::single);
  check_test(comm, mt::funneled != mt::serialized);
  check_test(comm, mt::funneled != mt::multiple);

  check_test(comm, mt::serialized != mt::single);
  check_test(comm, mt::serialized != mt::funneled);
  check_test(comm, mt::serialized != mt::multiple);

  check_test(comm, mt::multiple != mt::single);
  check_test(comm, mt::multiple != mt::funneled);
  check_test(comm, mt::multiple != mt::serialized);

  check_test(comm, mt::single < mt::funneled);
  check_test(comm, mt::funneled > mt::single);
  check_test(comm, mt::single < mt::serialized);
  check_test(comm, mt::serialized > mt::single);
  check_test(comm, mt::single < mt::multiple);
  check_test(comm, mt::multiple > mt::single);

  check_test(comm, mt::funneled < mt::serialized);
  check_test(comm, mt::serialized > mt::funneled);
  check_test(comm, mt::funneled < mt::multiple);
  check_test(comm, mt::multiple > mt::funneled);

  check_test(comm, mt::serialized < mt::multiple);
  check_test(comm, mt::multiple > mt::serialized);

  check_test(comm, mt::single <= mt::single);
  check_test(comm, mt::single <= mt::funneled);
  check_test(comm, mt::funneled >= mt::single);
  check_test(comm, mt::single <= mt::serialized);
  check_test(comm, mt::serialized >= mt::single);
  check_test(comm, mt::single <= mt::multiple);
  check_test(comm, mt::multiple >= mt::single);

  check_test(comm, mt::funneled <= mt::funneled);
  check_test(comm, mt::funneled <= mt::serialized);
  check_test(comm, mt::serialized >= mt::funneled);
  check_test(comm, mt::funneled <= mt::multiple);
  check_test(comm, mt::multiple >= mt::funneled);

  check_test(comm, mt::serialized <= mt::serialized);
  check_test(comm, mt::serialized <= mt::multiple);
  check_test(comm, mt::multiple >= mt::serialized);

  check_test(comm, mt::multiple <= mt::multiple);
}
    
int
main(int argc, char* argv[]) {
  mpi::environment env;
  mpi::communicator world;
  test_threading_levels_io(world);
  test_threading_level_cmp(world);
  return 0;
}
