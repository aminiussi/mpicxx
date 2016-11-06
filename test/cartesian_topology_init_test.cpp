
//          Copyright Alain Miniussi 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Authors: Alain Miniussi

#include <vector>
#include <list>
#include <iostream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <functional>

#include <boost/mpicxx/communicator.hpp>
#include <boost/mpicxx/collectives.hpp>
#include <boost/mpicxx/environment.hpp>
#include <boost/mpicxx/cartesian_communicator.hpp>

#include "check_test.hpp"

namespace mpi = boost::mpicxx;


int main(int argc, char const* argv)
{
  {
    // Check the basic ctor
    mpi::cartesian_dimension def;
    mpi::cartesian_topology t1(10);
    check_test(nocomm(), t1.stl() == std::vector<mpi::cartesian_dimension>(10, def));
  }
#if !defined(BOOST_NO_CXX11_HDR_INITIALIZER_LIST)
  {
    // Intializer list ctor vs range based
    int dims[] = {2,3,4};
    bool per[] = {true, false, true};
    mpi::cartesian_topology t1(dims, per);
    mpi::cartesian_topology t2({{2,true},{3, false},{4, true}});
    check_test(t1.size() == 3);
    check_test(t1 == t2);
  }
#endif
  // Container based ctor only available as a replacement for initializer list ctor
  {
    // seq ctor vs C array ctor
    mpi::cartesian_dimension d[] = {{2,true},{3, false},{4, true}};
    std::list<mpi::cartesian_dimension> seq;
    std::copy(d, d+3, std::back_inserter(seq));
    mpi::cartesian_topology t1(seq);
    mpi::cartesian_topology t2(d);
    check_test(nocomm(), t1 == t2);
  }
  {
    // Check range based with array based ctor.
    boost::array<mpi::cartesian_dimension, 3> d = {{{2,true},{3, false},{4, true}}};
    int dims[] = {2,3,4};
    bool per[] = {true, false, true};
    mpi::cartesian_topology t1(dims, per);
    mpi::cartesian_topology t2(d);
    check_test(nocomm(), t1.size() == 3);
    check_test(nocomm(), t1 == t2);
  }
  {
    // Iterator based ctor vs C array based ctor
    mpi::cartesian_dimension d[] = {{2,true},{3, false},{4, true}};
    std::vector<mpi::cartesian_dimension> vdims(d, d+3);
    mpi::cartesian_topology t1(vdims);
    mpi::cartesian_topology t2(d);
    check_test(nocomm(), t1.size() == 3);
    check_test(nocomm(), t1 == t2);
    check_test(nocomm(), !(t1 != t2));
    t1[1].periodic = true;
    check_test(nocomm(), t1 != t2);
    t1[2].periodic = false;
    t1[2].size = 0;
    vdims.push_back(mpi::cartesian_dimension(3, false));
    mpi::cartesian_topology t3(vdims);
    check_test(nocomm(), t1 != t3);
  }
}
