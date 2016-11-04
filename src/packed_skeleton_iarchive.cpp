// (C) Copyright 2005 Matthias Troyer

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  Authors: Matthias Troyer

#define BOOST_ARCHIVE_SOURCE

#include <boost/archive/detail/archive_serializer_map.hpp>
#include <boost/archive/impl/archive_serializer_map.ipp>
#include <boost/mpicxx/skeleton_and_content.hpp>

namespace boost { namespace archive {

// explicitly instantiate all required templates

// template class basic_binary_iarchive<mpi::packed_skeleton_iarchive> ;
template class detail::archive_serializer_map<mpicxx::packed_skeleton_iarchive> ;
template class detail::archive_serializer_map<
  mpicxx::detail::forward_skeleton_iarchive<
    boost::mpicxx::packed_skeleton_iarchive, boost::mpicxx::packed_iarchive> > ;

} } // end namespace boost::archive
