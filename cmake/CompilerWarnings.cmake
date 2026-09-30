# Applies the project's mandated warning set (see docs/SPEC.md §4) and,
# when TINYKV_SANITIZE is ON, ASan+UBSan to a single target.
function(tinykv_set_warnings target)
    target_compile_options(${target} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wconversion
        -Werror
    )

    if(TINYKV_SANITIZE)
        target_compile_options(${target} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer -g)
        target_link_options(${target} PRIVATE -fsanitize=address,undefined)
    endif()
endfunction()
