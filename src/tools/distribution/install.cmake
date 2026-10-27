find_package(Python3 REQUIRED COMPONENTS Interpreter)
find_program(YOSIC_POWERSHELL NAMES powershell pwsh REQUIRED)
set(dos_dir "${PROJECT_BINARY_DIR}/dos")
add_custom_target(dos-images ALL
    COMMAND "${YOSIC_POWERSHELL}" -NoProfile -ExecutionPolicy Bypass -File
        "${PROJECT_SOURCE_DIR}/dos/tools/build.ps1"
        -ToolDirectory "${PROJECT_BINARY_DIR}" -PythonPath "${Python3_EXECUTABLE}"
        -OutputDirectory "${dos_dir}"
    DEPENDS yosic-as
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/.."
    VERBATIM)

install(TARGETS yosic-sim yosic-as yosic-mkimage RUNTIME DESTINATION bin)
install(FILES
    "${PROJECT_BINARY_DIR}/boot/ipl.rom" "${PROJECT_BINARY_DIR}/boot/ipl.lst"
    "${PROJECT_BINARY_DIR}/boot/ipl.map" "${PROJECT_BINARY_DIR}/boot/stage2.bin"
    "${PROJECT_BINARY_DIR}/boot/stage2.lst" "${PROJECT_BINARY_DIR}/boot/stage2.map"
    "${PROJECT_BINARY_DIR}/boot/boot.img" "${PROJECT_BINARY_DIR}/boot/boot.d88"
    DESTINATION boot)
install(FILES "${dos_dir}/dos.img" "${dos_dir}/dos.d88"
    "${dos_dir}/dos-1.25.img" "${dos_dir}/dos-1.25.d88" DESTINATION dos)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/firmware" "${PROJECT_SOURCE_DIR}/examples"
    "${PROJECT_SOURCE_DIR}/dos" DESTINATION src
    PATTERN "tests" EXCLUDE PATTERN "test" EXCLUDE PATTERN "local-tests" EXCLUDE
    PATTERN "PLDULL" EXCLUDE PATTERN "__pycache__" EXCLUDE
    PATTERN "Agents.md" EXCLUDE PATTERN "build-tools.md" EXCLUDE
    PATTERN "*.pyc" EXCLUDE PATTERN "*.bin" EXCLUDE PATTERN "*.img" EXCLUDE
    PATTERN "*.d88" EXCLUDE PATTERN "*.kfd" EXCLUDE PATTERN "*.YKE" EXCLUDE)
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

