// Copyright (C) 2006 Douglas Gregor <doug.gregor -at- gmail.com>.

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// Message Passing Interface

//  See www.boost.org/libs/mpi for documentation.

/** @file mpi.hpp
 *
 *  This file is a top-level convenience header that includes all of
 *  the Boost.MPI library headers. Users concerned about compile time
 *  may wish to include only specific headers from the Boost.MPI
 *  library.
 *
 */
#ifndef BOOST_MPI_HPP
#define BOOST_MPI_HPP

#include <boost/mpicxx/allocator.hpp>
#include <boost/mpicxx/collectives.hpp>
#include <boost/mpicxx/communicator.hpp>
#include <boost/mpicxx/datatype.hpp>
#include <boost/mpicxx/environment.hpp>
#include <boost/mpicxx/graph_communicator.hpp>
#include <boost/mpicxx/group.hpp>
#include <boost/mpicxx/intercommunicator.hpp>
#include <boost/mpicxx/nonblocking.hpp>
#include <boost/mpicxx/operations.hpp>
#include <boost/mpicxx/skeleton_and_content.hpp>
#include <boost/mpicxx/timer.hpp>

#endif // BOOST_MPI_HPP
