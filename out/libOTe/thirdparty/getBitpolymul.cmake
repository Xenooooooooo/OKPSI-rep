set(CLONE_DIR "${OC_THIRDPARTY_CLONE_DIR}/bitpolymul")
set(BUILD_DIR "${CLONE_DIR}/out/build/${OC_CONFIG}")
set(LOG_FILE  "${CMAKE_CURRENT_LIST_DIR}/log-bitpolymul.txt")
set(CONFIG    --config ${CMAKE_BUILD_TYPE})


include("${CMAKE_CURRENT_LIST_DIR}/fetch.cmake")

if(NOT EXISTS ${BUILD_DIR} OR NOT BITPOLYMUL_FOUND)
    find_program(GIT git REQUIRED)
    set(CONFIGURE_CMD ${CMAKE_COMMAND} -S ${CLONE_DIR} -B ${BUILD_DIR} -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
                       -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
                        -DBITPOLYMUL_ASAN=${ENABLE_ASAN}
                        -DBITPOLYMUL_PIC=${ENABLE_PIC}
                        )
    set(BUILD_CMD     ${CMAKE_COMMAND} --build ${BUILD_DIR} ${CONFIG})
    set(INSTALL_CMD   ${CMAKE_COMMAND} --install ${BUILD_DIR} ${CONFIG} --prefix ${OC_THIRDPARTY_INSTALL_PREFIX})


    message("============= Building bitpolymul =============")

    run(NAME "Configure"       CMD ${CONFIGURE_CMD} WD ${CLONE_DIR})
    run(NAME "Build"           CMD ${BUILD_CMD}     WD ${CLONE_DIR})
    run(NAME "Install"         CMD ${INSTALL_CMD}   WD ${CLONE_DIR})

    message("log ${LOG_FILE}\n==========================================")
else()
    message("bitpolymul already fetched.")
endif()

install(CODE "
    if(NOT CMAKE_INSTALL_PREFIX STREQUAL \"${OC_THIRDPARTY_INSTALL_PREFIX}\")
        execute_process(
            COMMAND ${SUDO} \${CMAKE_COMMAND} --install \"${BUILD_DIR}\" ${CONFIG}  --prefix \${CMAKE_INSTALL_PREFIX}
            WORKING_DIRECTORY ${CLONE_DIR}
            RESULT_VARIABLE RESULT
            COMMAND_ECHO STDOUT
        )
    endif()
")
