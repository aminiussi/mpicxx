cmake_minimum_required (VERSION 3.1)
project (MPICxx)

enable_testing()

include_directories(include ../../local/include)

add_library(mpicxx 
  src/broadcast.cpp
  src/cartesian_communicator.cpp
  src/communicator.cpp
  src/computation_tree.cpp
  src/content_oarchive.cpp
  src/environment.cpp
  src/exception.cpp
  src/graph_communicator.cpp
  src/group.cpp
  src/ibroadcast.cpp
  src/intercommunicator.cpp
  src/mpi_datatype_cache.cpp
  src/mpi_datatype_oarchive.cpp
  src/nonblocking.cpp
  src/offsets.cpp
  src/packed_iarchive.cpp
  src/packed_oarchive.cpp
  src/packed_skeleton_iarchive.cpp
  src/packed_skeleton_oarchive.cpp
  src/point_to_point.cpp
  src/request.cpp
  src/text_skeleton_oarchive.cpp
  src/timer.cpp )

#find_package(Boost 1.40 
#  COMPONENTS serialization filesystem)
find_library(BOOST_SERIALIZATION
  boost_serialization
  HINTS ../../local/lib)

find_package(MPI)
if(EXISTS  "${MPIEXEC}.hydra")
  set(MPIEXEC "${MPIEXEC}.hydra")
  message(INFO "  Found MPIEXEC: ${MPIEXEC}")
endif()

include_directories(${MPI_INCLUDE_PATH})
target_link_libraries(mpicxx ${MPI_CXX_LIBRARIES} ${BOOST_SERIALIZATION})

add_subdirectory(test)
