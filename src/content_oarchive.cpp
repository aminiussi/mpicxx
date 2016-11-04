// (C) Copyright 2005 Matthias Troyer

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  Authors: Matthias Troyer

#define BOOST_ARCHIVE_SOURCE
#include <boost/archive/detail/archive_serializer_map.hpp>
#include <boost/archive/impl/archive_serializer_map.ipp>
#include <boost/mpicxx/skeleton_and_content.hpp>

namespace boost { namespace archive { namespace detail {
// explicitly instantiate all required template functions

template class archive_serializer_map<mpicxx::detail::content_oarchive> ;
template class archive_serializer_map<boost::mpicxx::detail::ignore_skeleton_oarchive<boost::mpicxx::detail::content_oarchive> >;
template class archive_serializer_map<boost::mpicxx::detail::ignore_skeleton_oarchive<boost::mpicxx::detail::mpi_datatype_oarchive> >;
} } }
