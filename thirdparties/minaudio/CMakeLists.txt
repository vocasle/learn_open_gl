add_library(minaudio minaudio.cpp)
target_include_directories(minaudio PUBLIC include)
target_link_libraries(minaudio pthread)