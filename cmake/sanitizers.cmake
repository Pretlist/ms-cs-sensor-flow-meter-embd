
#------------------------------------------------------------------------------
# Clang and gcc sanitizers
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    message(STATUS "Sanitizers:")

    # 1. ADDRESS SENITIZERS
    option(ADDRESS_SANITIZER "description" OFF)
    message(STATUS "  + ADDRESS_SANITIZER                     ${ADDRESS_SANITIZER}")
    if(ADDRESS_SANITIZER)
        add_compile_options(-fsanitize=address)
        link_libraries(-fsanitize=address)
        # REMOVE OPTIONS: -fsanitize=safe-stack should be off
        list(REMOVE_ITEM BUILD_TYPE_DEBUG "-fsanitize=safe-stack")
        list(REMOVE_ITEM BUILD_TYPE_DEBUG "-ffine-grained-bitfield-accesses")
        # The Address Sanitizer run-time is not linked, so -Wl,-z,defs
        # M ay cause link errors (don’t use it with AddressSanitizer)
        list(REMOVE_ITEM shared_linker_flags "-Wl,-z,defs")
    endif()

    # 2. UNDEFINED SENITIZERS
    option(UB_SANITIZER "description" OFF)
    message(STATUS "  + UB_SANITIZER                          ${UB_SANITIZER}")
    if(UB_SANITIZER)
        add_compile_options(-fsanitize=undefined)
        link_libraries(-fsanitize=undefined)
        # REMOVE OPTIONS: -fsanitize=safe-stack should be off
        list(REMOVE_ITEM BUILD_TYPE_DEBUG "-fsanitize=safe-stack")
        list(REMOVE_ITEM BUILD_TYPE_DEBUG "-ffine-grained-bitfield-accesses")
    endif()

    # 3. INTEGER SENITIZERS
    option(INTEGER_SANITIZER "description" OFF)
    message(STATUS "  + INTEGER_SANITIZER                     ${INTEGER_SANITIZER}")
    if(INTEGER_SANITIZER)
        add_compile_options(-fsanitize=integer)
        link_libraries(-fsanitize=integer)
        # REMOVE OPTIONS: -fsanitize=safe-stack should be off
        list(REMOVE_ITEM BUILD_TYPE_DEBUG "-fsanitize=safe-stack")
        list(REMOVE_ITEM BUILD_TYPE_DEBUG "-ffine-grained-bitfield-accesses")
    endif()

    # 4. IMPLICIT CONVERSION SANITIZER
    option(IMPLICIT_CONVERSION_SANITIZER "description" OFF)
    message(STATUS "  + IMPLICIT_CONVERSION_SANITIZER         ${IMPLICIT_CONVERSION_SANITIZER}")
    if(IMPLICIT_CONVERSION_SANITIZER)
        add_compile_options(-fsanitize=implicit-conversion)
        link_libraries(-fsanitize=implicit-conversion)
        # REMOVE OPTIONS: -fsanitize=safe-stack should be off
        list(REMOVE_ITEM BUILD_TYPE_DEBUG "-fsanitize=safe-stack")
        list(REMOVE_ITEM BUILD_TYPE_DEBUG "-ffine-grained-bitfield-accesses")
    endif()

    # 5. NULLABILITY SANITIZER
    option(NULLABILITY_SANITIZER "description" OFF)
    message(STATUS "  + NULLABILITY_SANITIZER                 ${NULLABILITY_SANITIZER}")
    if(NULLABILITY_SANITIZER)
        add_compile_options(-fsanitize=nullability)
        link_libraries(-fsanitize=nullability)
        # REMOVE OPTIONS: -fsanitize=safe-stack should be off
        list(REMOVE_ITEM BUILD_TYPE_DEBUG "-fsanitize=safe-stack")
        list(REMOVE_ITEM BUILD_TYPE_DEBUG "-ffine-grained-bitfield-accesses")
    endif()

    # 6. THREAD SANITIZER
    option(THREAD_SANITIZER "description" OFF)
    message(STATUS "  + THREAD_SANITIZER                      ${THREAD_SANITIZER}")
    if(THREAD_SANITIZER)
        add_compile_options(-fsanitize=undefined)
        link_libraries(-fsanitize=undefined)
        # REMOVE OPTIONS: -fsanitize=safe-stack should be off
        list(REMOVE_ITEM BUILD_TYPE_DEBUG "-fsanitize=safe-stack")
        list(REMOVE_ITEM BUILD_TYPE_DEBUG "-ffine-grained-bitfield-accesses")
    endif()

    # Clang only
    # 7. MEMORY SANITIZER
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        option(MEMORY_SANITIZER "description" OFF)
        message(STATUS "  + MEMORY_SANITIZER                      ${MEMORY_SANITIZER}")
        if(MEMORY_SANITIZER)
            set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-stdlib=libc++ -fsanitize=memory -fsanitize-memory-track-origins -O1")
            set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-stdlib=libc++ -fsanitize=memory -fsanitize-memory-track-origins -O1")
            add_compile_options(-fsanitize=memory -fsanitize-memory-track-origins=2 -O1 -stdlib=libc++ -L/libcxx_msan/lib)
            link_libraries(-fsanitize=memory -fsanitize-memory-track-origins=2 -O1 -stdlib=libc++ -L/libcxx_msan/lib)
            # REMOVE OPTIONS: -fsanitize=safe-stack should be off
            list(REMOVE_ITEM BUILD_TYPE_DEBUG "-fsanitize=safe-stack")
            list(REMOVE_ITEM BUILD_TYPE_DEBUG "-ffine-grained-bitfield-accesses")
        endif()

        # 8. CONTROL FLOW INTEGRITY SANITIZER
        option(CFI_SANITIZER "description" OFF)
        message(STATUS "  + CFI_SANITIZER                         ${CFI_SANITIZER}")
        if(CFI_SANITIZER)
            add_compile_options(-fsanitize=cfi-vcall -fno-sanitize-trap=cfi -fvisibility=default -flto)
            link_libraries(-fsanitize=cfi-vcall -fno-sanitize-trap=cfi -fvisibility=default -flto)
            # REMOVE OPTIONS: -fsanitize=safe-stack should be off
            list(REMOVE_ITEM BUILD_TYPE_DEBUG "-fsanitize=safe-stack")
            list(REMOVE_ITEM BUILD_TYPE_DEBUG "-ffine-grained-bitfield-accesses")
        endif()
    endif()
endif()

