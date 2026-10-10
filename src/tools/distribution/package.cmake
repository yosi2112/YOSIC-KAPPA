install(TARGETS yosic-sim yosic-as yosic-mkimage RUNTIME DESTINATION bin)
install(FILES
    "${PROJECT_BINARY_DIR}/boot/ipl.rom" "${PROJECT_BINARY_DIR}/boot/ipl.lst"
    "${PROJECT_BINARY_DIR}/boot/ipl.map" "${PROJECT_BINARY_DIR}/boot/stage2.bin"
    "${PROJECT_BINARY_DIR}/boot/stage2.lst" "${PROJECT_BINARY_DIR}/boot/stage2.map"
    "${PROJECT_BINARY_DIR}/boot/boot.img" "${PROJECT_BINARY_DIR}/boot/boot.d88"
    DESTINATION boot)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/firmware" "${PROJECT_SOURCE_DIR}/examples" DESTINATION src)
install(FILES
    "${PROJECT_SOURCE_DIR}/../doc/emulator.md"
    "${PROJECT_SOURCE_DIR}/../doc/assembler.md"
    "${PROJECT_SOURCE_DIR}/../doc/architecture-reference.md"
    "${PROJECT_SOURCE_DIR}/../doc/interface-reference.md"
    DESTINATION doc)
install(FILES "${PROJECT_SOURCE_DIR}/../readme.md" DESTINATION .)
install(FILES "${PROJECT_SOURCE_DIR}/../license" DESTINATION .)
install(CODE [=[
    file(GLOB_RECURSE payload LIST_DIRECTORIES false "${CMAKE_INSTALL_PREFIX}/*")
    list(SORT payload)
    set(manifest "")
    foreach(path IN LISTS payload)
        file(RELATIVE_PATH relative "${CMAKE_INSTALL_PREFIX}" "${path}")
        if(NOT relative STREQUAL "SHA256SUMS.txt")
            file(SHA256 "${path}" hash)
            string(APPEND manifest "${hash}  ${relative}\n")
        endif()
    endforeach()
    file(WRITE "${CMAKE_INSTALL_PREFIX}/SHA256SUMS.txt" "${manifest}")
]=])
set(CPACK_GENERATOR ZIP)
set(CPACK_PACKAGE_NAME yosic-kappa)
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(YOSIC_PACKAGE_FILE_NAME "yosic-kappa-${PROJECT_VERSION}-windows-x64-${CMAKE_BUILD_TYPE}")
set(CPACK_PACKAGE_FILE_NAME "${YOSIC_PACKAGE_FILE_NAME}")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY ON)
include(CPack)

