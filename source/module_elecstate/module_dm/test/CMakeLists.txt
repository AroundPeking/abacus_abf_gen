remove_definitions(-D__DEEPKS)
remove_definitions(-D__CUDA)
remove_definitions(-D__ROCM)
remove_definitions(-D__MPI)

install(DIRECTORY support DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

AddTest(
  TARGET dm_k_io
  LIBS ${math_libs} base device
  SOURCES test_dmk_io.cpp ../dmk_io.cpp tmp_mocks.cpp
)