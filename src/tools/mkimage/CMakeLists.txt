add_executable(yosic-mkimage main.cpp)
target_link_libraries(yosic-mkimage PRIVATE yosic-core)

set(boot_dir "${PROJECT_BINARY_DIR}/boot")
foreach(kind IN ITEMS ipl stage2)
    if(kind STREQUAL "ipl")
        set(extension rom)
    else()
        set(extension bin)
    endif()
    add_custom_command(OUTPUT "${boot_dir}/${kind}.${extension}"
        BYPRODUCTS "${boot_dir}/${kind}.lst" "${boot_dir}/${kind}.map"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${boot_dir}"
        COMMAND yosic-as "${PROJECT_SOURCE_DIR}/firmware/${kind}/${kind}.asm"
            -o "${boot_dir}/${kind}.${extension}" --list "${boot_dir}/${kind}.lst"
            --map "${boot_dir}/${kind}.map"
        DEPENDS yosic-as "${PROJECT_SOURCE_DIR}/firmware/${kind}/${kind}.asm"
        VERBATIM)
endforeach()
foreach(extension IN ITEMS img d88)
    add_custom_command(OUTPUT "${boot_dir}/boot.${extension}"
        COMMAND yosic-mkimage "${boot_dir}/stage2.bin" -o "${boot_dir}/boot.${extension}" --overwrite
        DEPENDS yosic-mkimage "${boot_dir}/stage2.bin" VERBATIM)
endforeach()
add_custom_target(boot-images ALL DEPENDS "${boot_dir}/ipl.rom"
    "${boot_dir}/boot.img" "${boot_dir}/boot.d88")
