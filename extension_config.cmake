# This file is included by DuckDB's build system. It specifies which extension
# to load

# Ensure dependencies are loaded before flock bootstraps config
duckdb_extension_load(core_functions)
duckdb_extension_load(json)

# Extension from this repo
duckdb_extension_load(flock SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR} LOAD_TESTS)
# Link flock into the shell and test binaries so it is loaded at startup
duckdb_extension_statically_link(flock)
