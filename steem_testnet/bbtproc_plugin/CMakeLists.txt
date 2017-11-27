file(GLOB HEADERS "include/steemit/plugins/bbtproc_api/*.hpp")

add_library( bbtproc_api
             ${HEADERS}
             bbtproc_api_plugin.cpp
           )

target_link_libraries( bbtproc_api steemit_app steemit_chain fc graphene_db )
target_include_directories( bbtproc_api
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )
