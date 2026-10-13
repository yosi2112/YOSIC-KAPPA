install(TARGETS yosic-sim yosic-as yosic-mkimage RUNTIME DESTINATION bin)
install(FILES
    "${PROJECT_BINARY_DIR}/boot/ipl.rom" "${PROJECT_BINARY_DIR}/boot/ipl.lst"
    "${PROJECT_BINARY_DIR}/boot/ipl.map" "${PROJECT_BINARY_DIR}/boot/stage2.bin"
    "${PROJECT_BINARY_DIR}/boot/stage2.lst" "${PROJECT_BINARY_DIR}/boot/stage2.map"
    "${PROJECT_BINARY_DIR}/boot/boot.img" "${PROJECT_BINARY_DIR}/boot/boot.d88"
    DESTINATION boot)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/firmware" "${PROJECT_SOURCE_DIR}/examples"
    "${PROJECT_SOURCE_DIR}/dos" DESTINATION src)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/../doc/" DESTINATION doc)
install(FILES "${PROJECT_SOURCE_DIR}/../readme.md" DESTINATION .)
install(FILES "${PROJECT_SOURCE_DIR}/../license" DESTINATION .)
install(CODE [=[
    get_filename_component(yosic_install_root "${CMAKE_INSTALL_PREFIX}" ABSOLUTE)
    file(GLOB_RECURSE payload LIST_DIRECTORIES false "${yosic_install_root}/*")
    list(SORT payload)
    set(manifest "")
    foreach(path IN LISTS payload)
        file(RELATIVE_PATH relative "${yosic_install_root}" "${path}")
        if(NOT relative STREQUAL "SHA256SUMS.txt")
            file(SHA256 "${path}" hash)
            string(APPEND manifest "${hash}  ${relative}\n")
        endif()
    endforeach()
    file(WRITE "${yosic_install_root}/SHA256SUMS.txt" "${manifest}")
]=])

