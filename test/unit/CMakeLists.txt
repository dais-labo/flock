find_package(GTest CONFIG REQUIRED)

file(GLOB_RECURSE TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
list(FILTER TEST_SOURCES EXCLUDE REGEX ".*test_main\\.cpp$")

file(COPY unit_test.db DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(${PROJECT_NAME}_tests test_main.cpp ${TEST_SOURCES})

# DuckDB's helper.hpp redefines __restrict on macOS, breaking gtest's later
# <regex.h>
if(APPLE)
  target_compile_options(${PROJECT_NAME}_tests PRIVATE -include regex.h)
endif()

# GNU ld (Linux + MinGW) needs --start-group/--end-group for circular deps
target_link_libraries(
  ${PROJECT_NAME}_tests
  PRIVATE GTest::gtest
          GTest::gmock
          $<$<CXX_COMPILER_ID:GNU>:-Wl,--start-group>
          ${PROJECT_NAME}_extension
          core_functions_extension
          json_extension
          parquet_extension
          duckdb_static
          $<$<CXX_COMPILER_ID:GNU>:-Wl,--end-group>)
link_extension_libraries(${PROJECT_NAME}_tests PRIVATE)

add_test(AllTestsInMain ${PROJECT_NAME}_tests)
