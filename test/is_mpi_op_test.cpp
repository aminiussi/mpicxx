// Copyright (C) 2005-2006 Douglas Gregor <doug.gregor@gmail.com>

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// A test of the is_mpi_op functionality.
#include <boost/mpicxx/operations.hpp>
#include <boost/mpicxx/environment.hpp>
#include <boost/type_traits/is_base_and_derived.hpp>
#include "check_test.hpp"

namespace mpi = boost::mpicxx;
using namespace std;
using boost::is_base_and_derived;

int main(int argc, char* argv[])
{
  boost::mpicxx::environment env(argc, argv);
  mpi::communicator comm;
  // Check each predefined MPI_Op type that we support directly.
  check_test(comm, (mpi::is_mpi_op<mpi::maximum<int>, int>::op() == MPI_MAX));
  check_test(comm, (mpi::is_mpi_op<mpi::minimum<float>, float>::op() == MPI_MIN));
  check_test(comm, (mpi::is_mpi_op<std::plus<double>, double>::op() == MPI_SUM));
  check_test(comm, (mpi::is_mpi_op<std::multiplies<long>, long>::op() == MPI_PROD));
  check_test(comm, (mpi::is_mpi_op<std::logical_and<int>, int>::op() == MPI_LAND));
  check_test(comm, (mpi::is_mpi_op<mpi::bitwise_and<int>, int>::op() == MPI_BAND));
  check_test(comm, (mpi::is_mpi_op<std::logical_or<int>, int>::op() == MPI_LOR));
  check_test(comm, (mpi::is_mpi_op<mpi::bitwise_or<int>, int>::op() == MPI_BOR));
  check_test(comm, (mpi::is_mpi_op<mpi::logical_xor<int>, int>::op() == MPI_LXOR));
  check_test(comm, (mpi::is_mpi_op<mpi::bitwise_xor<int>, int>::op() == MPI_BXOR));

  return 0;
}
