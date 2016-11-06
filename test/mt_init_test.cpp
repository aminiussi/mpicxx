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

int
main(int argc, char* argv[]) {
  mpi::threading::level required = mpi::threading::level(-1);
  assert(argc == 2);
  std::istringstream cmdline(argv[1]);
  cmdline >> required;
  assert(!cmdline.bad());
  mpi::environment env(argc,argv,required);
  mpi::communicator comm;
  check_test(comm, env.thread_level() >= mpi::threading::single);
  check_test(comm, env.thread_level() <= mpi::threading::multiple);
  return 0;
}
