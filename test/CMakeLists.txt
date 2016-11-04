function(add_simple_mpi_test basename)
  add_executable(${basename} ${basename}.cpp)
  target_link_libraries(${basename} mpicxx)
  foreach(nproc ${ARGN})
    add_test(${basename}_${nproc} ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} ${nproc} ${CMAKE_CURRENT_BINARY_DIR}/${basename})
  endforeach()
endfunction()

add_simple_mpi_test(ibroadcast_test 2 7 8)
add_simple_mpi_test(all_gather_test 2 7 8)
add_simple_mpi_test(all_reduce_test 3 7 8)

