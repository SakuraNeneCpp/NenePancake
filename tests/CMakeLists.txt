add_executable(sandbox sandbox.cpp)
target_link_libraries(sandbox PRIVATE NenePancake::NenePancake)
add_test(NAME sandbox COMMAND sandbox)