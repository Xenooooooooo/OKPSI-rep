
file(GLOB_RECURSE SRCS *.cpp)

add_executable(frontend  ${SRCS})

#############################################
#            Install  Myblake3              #
#############################################
# Your-external "mylib", add GLOBAL if the imported library is located in directories above the current.
add_library(myblake3 SHARED IMPORTED GLOBAL)
# You can define two import-locations: one for debug and one for release.
# set_target_properties(myblake3 PROPERTIES IMPORTED_LOCATION /home/lx1234/work/volepsi/out/install/linux/lib/libblake3.so)
set_target_properties(myblake3 PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/out/install/linux/lib/libblake3.so)

target_link_libraries(frontend volePSI_Tests myblake3)


if(VOLE_PSI_NO_LTO)
    target_link_options(frontend PUBLIC "-fno-lto")
endif()



if(MSVC)
    target_compile_options(frontend PRIVATE $<$<COMPILE_LANGUAGE:CXX>:/std:c++${VOLE_PSI_STD_VER}>)
    #target_compile_options(libOTe PRIVATE -openmp:experimental)
else()
    target_compile_options(frontend PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-std=c++${VOLE_PSI_STD_VER}>)
endif()