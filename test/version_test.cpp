// Copyright (C) 2013 Alain Miniussi <alain.miniussi@oca.eu>

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// test mpi version

#include <boost/mpicxx/environment.hpp>
#include "check_test.hpp"
#include <iostream>

namespace mpi = boost::mpicxx;

int
main(int argc, char* argv[]) {
#if defined(MPI_VERSION)
  int mpi_version    = MPI_VERSION;
  int mpi_subversion = MPI_SUBVERSION;
#else
  int mpi_version = 0;
  int mpi_subversion = 0;
#endif

  mpi::environment env(argc,argv);
  mpi::communicator comm;
  std::pair<int,int> version = env.version();
  std::cout << "MPI Version: " << version.first << ',' << version.second << '\n';

  check_test(comm, version.first == mpi_version);
  check_test(comm, version.second == mpi_subversion);

#if defined(BOOST_MPI_HAS_NONBLOCKING_GLOBAL)
  std::cout << "Assuming non-blocking globals availables.\n";
  if (false) { // are those defined ?
    std::cout << MPI_Ibcast(0, 0, MPI_INTEGER, 0, MPI_COMM_WORLD, 0);
  }
#endif
  return 0;
}
