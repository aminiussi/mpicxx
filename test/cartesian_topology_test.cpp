//          Copyright Alain Miniussi 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Authors: Alain Miniussi

#include <vector>
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

struct topo_minimum {
  mpi::cartesian_dimension
  operator()(mpi::cartesian_dimension const& d1,
             mpi::cartesian_dimension const& d2 ) const {
    return mpi::cartesian_dimension(std::min(d1.size, d2.size),
                                    d1.periodic && d2.periodic);
  }
};

std::string topology_description( mpi::cartesian_topology const&  topo ) {
  std::ostringstream out;
  std::copy(topo.begin(), topo.end(), std::ostream_iterator<mpi::cartesian_dimension>(out, " "));
  out << std::flush;
  return out.str();
}

// Check that everyone agrees on the coordinates
void test_coordinates_consistency( mpi::cartesian_communicator const& cc,
                                   std::vector<int> const& coords )
{
  cc.barrier(); // flush IOs for nice printing
  bool master = cc.rank() == 0;
  if (master) {
    std::cout << "Test coordinates consistency.\n";
  }
  for(int p = 0; p < cc.size(); ++p) {
    std::vector<int> min(cc.ndims());
    std::vector<int> local(cc.coordinates(p));
    mpi::reduce(cc, local.data(), local.size(),
                min.data(), mpi::minimum<int>(), p);
    cc.barrier();
    if (p == cc.rank()) {
      check_test(cc, std::equal(coords.begin(), coords.end(), min.begin()));
      std::ostringstream out;
      out << "proc " << p << " at (";
      std::copy(min.begin(), min.end(), std::ostream_iterator<int>(out, " "));
      out << ")\n";
      std::cout << out.str();
    }
  }
}

void test_shifted_coords( mpi::cartesian_communicator const& cc, int pos,  mpi::cartesian_dimension desc, int dim )
{
  if (desc.periodic) {
    for (int i = -(desc.size); i < desc.size; ++i) {
      std::pair<int,int> rks = cc.shifted_ranks(dim, i);
      int src = cc.coordinates(rks.first)[dim];
      int dst = cc.coordinates(rks.second)[dim];
      if (pos == (dim/2)) {
        std::ostringstream out;
        out << "Rank " << cc.rank() << ", dim. " << dim << ", pos " << pos << ", in " << desc << ' ';
        out << "shifted pos: " << src << ", " << dst << '\n';
        std::cout << out.str();
      }
    }
  }
}

void test_shifted_coords( mpi::cartesian_communicator const& cc)
{
  cc.barrier(); // flush IOs for nice printing
  std::vector<int> coords; 
  mpi::cartesian_topology topo(cc.ndims());
  cc.topology(topo, coords);
  bool master = cc.rank() == 0;
  if (master) {
    std::cout << "Testing shifts with topology " << topo << '\n';
  }
  for(int i = 0; i < cc.ndims(); ++i) {
    if (master) {
      std::cout << " for dimension " << i << ": " << topo[i] << '\n';
    }
    test_shifted_coords( cc, coords[i], topo[i], i );
  }
}

void test_topology_consistency( mpi::cartesian_communicator const& cc) 
{
  cc.barrier(); // flush IOs for nice printing
  mpi::cartesian_topology itopo(cc.ndims());
  mpi::cartesian_topology otopo(cc.ndims());
  std::vector<int>  coords(cc.ndims());
  cc.topology(itopo, coords);
  bool master = cc.rank() == 0;
  if (master) {
    std::cout << "Test topology consistency of" << itopo << "(on master)\n";
    std::cout << "Check that everyone agrees on the dimensions.\n";
  }
  mpi::all_reduce(cc, 
                  &(itopo[0]), itopo.size(), &(otopo[0]),
                  topo_minimum());
  check_test(cc, std::equal(itopo.begin(), itopo.end(), otopo.begin()));
  if (master) {
    std::cout << "We agree on " << topology_description(otopo) << '\n';
  }
  test_coordinates_consistency( cc, coords );
}

void test_cartesian_topology( mpi::cartesian_communicator const& cc)
{
  check_test(cc, cc.has_cartesian_topology());
  for( int r = 0; r < cc.size(); ++r) {
    cc.barrier();
    if (r == cc.rank()) {
      std::vector<int> coords = cc.coordinates(r);
      std::cout << "Process of cartesian rank " << cc.rank() 
                << " has coordinates (";
      std::copy(coords.begin(), coords.end(), std::ostream_iterator<int>(std::cout," "));
      std::cout << ")\n";
    }
  }
  test_topology_consistency(cc);
  test_shifted_coords(cc);
  std::vector<int> even;
  for(int i = 0; i < cc.ndims(); i += 2) {
    even.push_back(i);
  }
  cc.barrier();
  mpi::cartesian_communicator cce(cc, even);
}

void test_cartesian_topology( mpi::communicator const& world, mpi::cartesian_topology const& topo) 
{
  mpi::cartesian_communicator cc(world, topo, true);
  if (cc) {
    check_test(world, cc.has_cartesian_topology());
    check_test(world, cc.ndims() == int(topo.size()));
    if (cc.rank() == 0) {
      std::cout << "Asked topology " << topo << ", got " << cc.topology() << '\n';
    }
    test_cartesian_topology(cc);
  } else {
    std::ostringstream out;
    out << world.rank() << " was left outside the cartesian grid\n";
    std::cout << out.str();
  }
}

int main(int argc, char* argv[])
{
  mpi::environment env(argc, argv);

  mpi::communicator world;
  int const ndim = world.size() >= 24 ? 3 : 2;
  mpi::cartesian_topology topo(ndim);
  typedef mpi::cartesian_dimension cd;
  if (topo.size() == 3) {
    topo[0] = cd(2,true);
    topo[1] = cd(3,false);
    topo[2] = cd(4, true);
  } else {
    if (world.size() >= 6) {
      topo[0] = cd(2,true);
      topo[1] = cd(3, false);
    } else {
      topo[0] = cd(1,true);
      topo[1] = cd(1, false);
    }
  }
  test_cartesian_topology( world, topo);

  return 0;
}
