// Copyright (C) 2013 Andreas Hehn <hehn@phys.ethz.ch>, ETH Zurich

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// A test of communicators created from groups.

#include <boost/mpicxx/environment.hpp>
#include <boost/mpicxx/communicator.hpp>
#include <boost/mpicxx/group.hpp>
#include "check_test.hpp"
#include <vector>
#include <algorithm>

namespace mpi = boost::mpicxx;


template <typename T>
struct iota
{
    iota() : state(0){};
    T operator()()
    {
        return state++;
    }
    T state;
};

void group_test(const mpi::communicator& comm)
{
    std::vector<int> grp_a_ranks(comm.size() / 2);
    std::generate(grp_a_ranks.begin(),grp_a_ranks.end(),iota<int>());

    mpi::group grp_a = comm.group().include(grp_a_ranks.begin(),grp_a_ranks.end());
    mpi::group grp_b = comm.group().exclude(grp_a_ranks.begin(),grp_a_ranks.end());

    mpi::communicator part_a(comm,grp_a);
    mpi::communicator part_b(comm,grp_b);

    if(part_a)
    {
        std::cout << "comm rank: " << comm.rank() << " -> part_a rank:" << part_a.rank() << std::endl;
        check_test(comm, part_a.rank() == comm.rank());
    }
    if(part_b)
    {
        std::cout << "comm rank: " << comm.rank() << " -> part_b rank:" << part_b.rank() << std::endl;
        check_test(comm, part_b.rank() == comm.rank() - comm.size()/2);
    }
}

int main(int argc, char* argv[])
{
    mpi::environment env(argc,argv);
    mpi::communicator comm;
    group_test(comm);
    return 0;
}
