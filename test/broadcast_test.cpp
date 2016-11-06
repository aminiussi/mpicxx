// Copyright (C) 2005, 2006 Douglas Gregor.

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// A test of the broadcast() collective.
#include <boost/mpicxx/collectives/broadcast.hpp>
#include <boost/mpicxx/communicator.hpp>
#include <boost/mpicxx/environment.hpp>
#include <algorithm>
#include "gps_position.hpp"
#include <boost/serialization/string.hpp>
#include <boost/serialization/list.hpp>
#include <boost/mpicxx/skeleton_and_content.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include "check_test.hpp"
//#include "debugger.hpp"

using boost::mpicxx::communicator;

using boost::mpicxx::packed_skeleton_iarchive;
using boost::mpicxx::packed_skeleton_oarchive;

template<typename T>
void
broadcast_test(const communicator& comm, const T& bc_value,
               const char* kind, int root = -1)
{
  if (root == -1) {
    for (root = 0; root < comm.size(); ++root)
      broadcast_test(comm, bc_value, kind, root);
  } else {
    using boost::mpicxx::broadcast;

    T value;
    if (comm.rank() == root) {
      value = bc_value;
      std::cout << "Broadcasting " << kind << " from root " << root << "...";
      std::cout.flush();
    }

    broadcast(comm, value, root);
    check_test(comm, value == bc_value);
  }

  (comm.barrier)();
}

void
test_skeleton_and_content(const communicator& comm, int root = 0)
{
  using boost::mpicxx::content;
  using boost::mpicxx::get_content;
  using boost::make_counting_iterator;
  using boost::mpicxx::broadcast;

  int list_size = comm.size() + 7;
  if (comm.rank() == root) {
    // Fill in the seed data
    std::list<int> original_list;
    for (int i = 0; i < list_size; ++i)
      original_list.push_back(i);

    // Build up the skeleton
    packed_skeleton_oarchive oa(comm);
    oa << original_list;

    // Broadcast the skeleton
    std::cout << "Broadcasting integer list skeleton from root " << root
              << "..." << std::flush;
    broadcast(comm, oa, root);
    std::cout << "OK." << std::endl;

    // Broadcast the content
    std::cout << "Broadcasting integer list content from root " << root
              << "..." << std::flush;
    {
      content c = get_content(original_list);
      broadcast(comm, c, root);
    }
    std::cout << "OK." << std::endl;

    // Reverse the list, broadcast the content again
    std::reverse(original_list.begin(), original_list.end());
    std::cout << "Broadcasting reversed integer list content from root "
              << root << "..." << std::flush;
    {
      content c = get_content(original_list);
      broadcast(comm, c, root);
    }
    std::cout << "OK." << std::endl;

  } else {
    // Allocate some useless data, to try to get the addresses of the
    // list<int>'s used later to be different across processes.
    std::list<int> junk_list(comm.rank() * 3 + 1, 17);

    // Receive the skeleton
    packed_skeleton_iarchive ia(comm);
    broadcast(comm, ia, root);

    // Build up a list to match the skeleton, and make sure it has the
    // right structure (we have no idea what the data will be).
    std::list<int> transferred_list;
    ia >> transferred_list;
    check_test(comm, int(transferred_list.size()) == list_size);

    // Receive the content and check it
    broadcast(comm, get_content(transferred_list), root);
    bool list_content_ok = std::equal(make_counting_iterator(0),
				      make_counting_iterator(list_size),
				      transferred_list.begin());
    check_test(comm, list_content_ok);

    // Receive the reversed content and check it
    broadcast(comm, get_content(transferred_list), root);
    bool rlist_content_ok = std::equal(make_counting_iterator(0),
				       make_counting_iterator(list_size),
				       transferred_list.rbegin());
    check_test(comm, rlist_content_ok);
    if (!(list_content_ok && rlist_content_ok)) {
      if (comm.rank() == 1) {
	std::cout
	  << "\n##### You might want to check for BOOST_MPI_BCAST_BOTTOM_WORKS_FINE "
	  << "in boost/mpi/config.hpp.\n\n";
      }
    }
  }

  (comm.barrier)();
}

int main(int argc, char* argv[])
{
  boost::mpicxx::environment env(argc, argv);

  communicator comm;
  if (comm.size() == 1) {
    std::cerr << "ERROR: Must run the broadcast test with more than one "
              << "process." << std::endl;
    MPI_Abort(comm, -1);
  }

  //wait_for_debugger(extract_paused_ranks(argc, argv), comm);

  // Check transfer of individual objects
  broadcast_test(comm, 17, "integers");
  broadcast_test(comm, gps_position(39,16,20.2799), "GPS positions");
  broadcast_test(comm, gps_position(26,25,30.0), "GPS positions");
  broadcast_test(comm, std::string("Rosie"), "string");

  std::list<std::string> strings;
  strings.push_back("Hello");
  strings.push_back("MPI");
  strings.push_back("World");
  broadcast_test(comm, strings, "list of strings");

  test_skeleton_and_content(comm, 0);
  test_skeleton_and_content(comm, 1);
  return 0;
}
