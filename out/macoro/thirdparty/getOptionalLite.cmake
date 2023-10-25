
set(DEP_NAME            optional-lite)

set(CLONE_DIR "${MACORO_THIRDPARTY_CLONE_DIR}/${DEP_NAME}")
set(BUILD_DIR "${CLONE_DIR}/out/build/${MACORO_CONFIG}")
set(LOG_FILE  "${CMAKE_CURRENT_LIST_DIR}/log-${DEP_NAME}.txt")
set(CONFIG    --config ${CMAKE_BUILD_TYPE})


include("${CMAKE_CURRENT_LIST_DIR}/fetch.cmake")

if(NOT optional-lite_FOUND)
    find_program(GIT git REQUIRED)
    set(CONFIGURE_CMD ${CMAKE_COMMAND} -S ${CLONE_DIR} -B ${BUILD_DIR} -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
                       -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE} -DOPTIONAL_LITE_OPT_BUILD_TESTS=OFF)
    set(BUILD_CMD     ${CMAKE_COMMAND} --build ${BUILD_DIR} ${CONFIG})
    set(INSTALL_CMD   ${CMAKE_COMMAND} --install ${BUILD_DIR} ${CONFIG} --prefix ${MACORO_STAGE})


    message("============= Building ${DEP_NAME} =============")

    run(NAME "Configure"       CMD ${CONFIGURE_CMD} WD ${CLONE_DIR})
    run(NAME "Build"           CMD ${BUILD_CMD}     WD ${CLONE_DIR})
    run(NAME "Install"         CMD ${INSTALL_CMD}   WD ${CLONE_DIR})

    message("log ${LOG_FILE}\n==========================================")
else()
    message("${DEP_NAME} already fetched.")
endif()

install(CODE "


    if(NOT CMAKE_INSTALL_PREFIX STREQUAL \"${MACORO_STAGE}\")
        execute_process(
            COMMAND ${SUDO} \"${CMAKE_COMMAND}\" --install \"${BUILD_DIR}\" --prefix \${CMAKE_INSTALL_PREFIX}
            WORKING_DIRECTORY ${CLONE_DIR}
            RESULT_VARIABLE RESULT
            COMMAND_ECHO STDOUT
        )
    endif()
")



