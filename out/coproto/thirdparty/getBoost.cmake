

set(BOOST_VERSION "77")
set(BOOST_NAME    "boost_1_${BOOST_VERSION}_0")
set(CLONE_DIR     "${COPROTO_THIRDPARTY_CLONE_DIR}/${BOOST_NAME}")
set(ARCH_PATH     "${COPROTO_THIRDPARTY_CLONE_DIR}/${BOOST_NAME}.tar.bz2")
set(INSTALL_DIR   ${COPROTO_STAGE})
set(URL           "https://boostorg.jfrog.io/artifactory/main/release/1.${BOOST_VERSION}.0/source/boost_1_${BOOST_VERSION}_0.tar.bz2")
set(LOG_FILE      "${CMAKE_CURRENT_LIST_DIR}/log-boost.txt")
set(SHA256_VAL    fc9f85fc030e233142908241af7a846e60630aa7388de9a5fafb1f3a26840854)

include("${CMAKE_CURRENT_LIST_DIR}/fetch.cmake")


if(MSVC)
    set(BOOTSTRAP "${CLONE_DIR}/bootstrap.bat")
    set(B2 "${CLONE_DIR}/b2.exe")
else()
    set(BOOTSTRAP "${CLONE_DIR}/bootstrap.sh")
    set(B2 "${CLONE_DIR}/b2")
endif()

SET(B2_ARGS 
        --with-system
        --with-thread 
        --with-atomic
        --with-filesystem
        --with-regex
        --with-date_time
        -j ${PARALLEL_FETCH})

if(MSVC)

    set(FINDVS_PATH "${CMAKE_CURRENT_LIST_DIR}/findvs.ps1")
    set(TEMP_PATH "${COPROTO_THIRDPARTY_CLONE_DIR}/buildBoost_deleteMe.ps1")
    list(APPEND B2_ARGS 
            toolset=msvc-14.2
            architecture=x86 
            address-model=64
            )
    
    if(Boost_USE_MULTITHREADED)
        list(APPEND B2_ARGS threading=multi)
    endif()

    if(Boost_USE_STATIC_LIBS)
        list(APPEND B2_ARGS link=static)
    endif()

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        list(APPEND B2_ARGS variant=debug)
    else()
        list(APPEND B2_ARGS variant=release)
    endif()

endif()


if(NOT EXISTS ${CLONE_DIR} OR NOT Boost_FOUND)

    message("============= Building Boost =============")
    if(NOT EXISTS ${CLONE_DIR})


        message("downloading ${URL}")

        file(DOWNLOAD ${URL} ${ARCH_PATH} 
            INACTIVITY_TIMEOUT 20
            EXPECTED_HASH SHA256=${SHA256_VAL}
            STATUS BOOST_DOWNLOAD_RESULT
            SHOW_PROGRESS
            )

        list(GET ${BOOST_DOWNLOAD_RESULT} 0 BOOST_DOWNLOAD_RESULT_CODE)
        if((NOT DEFINED BOOST_DOWNLOAD_RESULT) OR BOOST_DOWNLOAD_RESULT_CODE)
            message(FATAL_ERROR "failed to download boost: ${BOOST_DOWNLOAD_RESULT}")
        endif()

        message("extracting")
        file(ARCHIVE_EXTRACT INPUT ${ARCH_PATH} DESTINATION ${COPROTO_THIRDPARTY_CLONE_DIR})
        
        file(REMOVE ${ARCH_PATH})
    endif()


    if(MSVC)
        set(B2 "${CLONE_DIR}/b2.exe")

        list(JOIN B2_ARGS " " B2_ARGS)
        vsrun(NAME "boost-build" CMD 
            "${BOOTSTRAP}\n"
            "${B2} ${B2_ARGS} install --prefix=${INSTALL_DIR}\n"
            WD ${CLONE_DIR}
            )
            
    else()
        set(B2 "${CLONE_DIR}/b2")

        run(NAME "configure" CMD "bash" "${BOOTSTRAP}" WD ${CLONE_DIR})

        if(DEFINED SUDO)
            run(NAME "build" CMD "${B2}" "${B2_ARGS}" WD ${CLONE_DIR})
        endif()

        run(NAME "install" CMD "${B2}" ${B2_ARGS} install --prefix=${INSTALL_DIR} WD ${CLONE_DIR})
    endif()



    message("log ${LOG_FILE}\n==========================================")
else()
    message("Boost already fetched. ${Boost_LIBRARIES} ")
endif()

if(MSVC)
    list(JOIN B2_ARGS " " B2_ARGS)
    install(CODE "
        if(NOT CMAKE_INSTALL_PREFIX STREQUAL \"${COPROTO_STAGE}\")

            include(${CMAKE_CURRENT_LIST_DIR}/fetch.cmake)
            vsrun(NAME install-boost CMD             
                \"${B2} ${B2_ARGS} install --prefix=\${CMAKE_INSTALL_PREFIX}\n\"
                WD ${CLONE_DIR})
        endif()
    ")
else()
    install(CODE "
        if(NOT CMAKE_INSTALL_PREFIX STREQUAL \"${COPROTO_STAGE}\")
            execute_process(
                COMMAND ${SUDO} \"${B2}\" ${B2_ARGS} install --prefix=\${CMAKE_INSTALL_PREFIX}
                WORKING_DIRECTORY ${CLONE_DIR}
                RESULT_VARIABLE RESULT
                COMMAND_ECHO STDOUT
            )
        endif()
    ")
endif()