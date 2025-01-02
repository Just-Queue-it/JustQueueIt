message("processing ${BINARY_DIR}")
file(GLOB_RECURSE COVERAGE_PROFRAW_FILES ${BINARY_DIR}/**/*.profraw)
file(GLOB_RECURSE COVERAGE_OBJECTS ${BINARY_DIR}/bin/*)
list(FILTER COVERAGE_OBJECTS EXCLUDE REGEX ".*\\.a")
set(COVERAGE_OBJECTS_RES "")

foreach(f ${COVERAGE_OBJECTS})
  list(APPEND COVERAGE_OBJECTS_RES -object=${f})
endforeach()

set(COVERAGE_ADDITIONAL_ARGS -instr-profile=${BINARY_DIR}/coverage.profdata
                             ${COVERAGE_OBJECTS_RES})

execute_process(COMMAND llvm-profdata merge -sparse=true ${COVERAGE_PROFRAW_FILES} -o ${BINARY_DIR}/coverage.profdata)

execute_process(
  COMMAND
    llvm-cov show --ignore-filename-regex=build --show-branches=count
    --show-expansions --show-line-counts --show-line-counts-or-regions
    --show-regions ${COVERAGE_ADDITIONAL_ARGS}
  OUTPUT_VARIABLE RES
  OUTPUT_STRIP_TRAILING_WHITESPACE)

file(WRITE "${BINARY_DIR}/coverage.txt" "${RES}")
