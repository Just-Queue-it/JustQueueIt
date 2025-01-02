add_custom_target(
  coverage
  COMMAND ${CMAKE_COMMAND} ARGS -DBINARY_DIR=${PROJECT_BINARY_DIR} -P
          ${CMAKE_SOURCE_DIR}/cmake/coverage_impl.cmake
  VERBATIM)
