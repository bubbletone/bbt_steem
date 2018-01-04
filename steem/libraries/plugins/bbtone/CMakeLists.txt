file(GLOB HEADERS "include/steemit/bbtone/*.hpp")

add_library( steemit_bbtone
             bbtone_plugin.cpp
             bbtone_evaluator.cpp
             bbtone_operations.cpp
           )

target_link_libraries( steemit_bbtone steemit_chain steemit_protocol steemit_app )
target_include_directories( steemit_bbtone
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )

install( TARGETS
   steemit_bbtone

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
