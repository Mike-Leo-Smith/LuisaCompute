set(LUISA_COMPUTE_AST_SOURCES
    function.h function.cpp
    function_builder.cpp function_builder.h
    expression.cpp expression.h
    variable.h
    statement.h
    type.cpp type.h
    type_registry.h
    interface.h
    constant_data.cpp constant_data.h
    op.h usage.h)

add_library(luisa-compute-ast SHARED ${LUISA_COMPUTE_AST_SOURCES})
target_link_libraries(luisa-compute-ast PUBLIC luisa-compute-core)
set_target_properties(luisa-compute-ast PROPERTIES
                      WINDOWS_EXPORT_ALL_SYMBOLS ON
                      UNITY_BUILD ON)
