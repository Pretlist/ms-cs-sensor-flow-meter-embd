# GET CMAKE ARGS

include(ExternalProject)

#
#   Add external project.
#
#   \param name             Name of external project
#   \param path             Path to source directory
#   \param external         Name of the external target
#
macro(add_ext_lib lib-name-interface configure-cmd module lib-name dep_lib build_type)
    # Create external project
    set(${module}_SOURCE_DIR ${THIRD_PARTY_DIR}/${module})
    set(${module}_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${module})
    set(${module}_CMAKE_ARGS ${CMAKE_ARGS})
    set(${module}_INSTALL_DIR ${CMAKE_INSTALL_PREFIX})
    message("INSTALL DIR CUSTOM PRJ = " ${${module}_INSTALL_DIR})
    message(" SOURCE DIR CUSTOM PRJ = " ${${module}_SOURCE_DIR})
    message("    BIN DIR CUSTOM PRJ = " ${${module}_BINARY_DIR})
    ExternalProject_Add(${module}
        SOURCE_DIR "${${module}_SOURCE_DIR}"
        BINARY_DIR "${${module}_BINARY_DIR}"
        #BUILD_IN_SOURCE 1
        BUILD_ALWAYS 1
        PATCH_COMMAND ${CMAKE_SOURCE_DIR}/patches/patch-manager.sh ${module} "${${module}_SOURCE_DIR}"
        CONFIGURE_COMMAND "${configure-cmd}"
        DEPENDS ${dep_lib}
        BUILD_COMMAND make
        INSTALL_DIR "${${module}_INSTALL_DIR}"
        INSTALL_COMMAND make install
    )

    ExternalProject_Add_Step(
        ${module} UNIX_CONFIGURE
        ALWAYS 1
        COMMENT "Copying source to build directory (make build) ${module}"
        DEPENDEES patch
        DEPENDERS configure
        COMMAND cp -ur "${${module}_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}"
    )

    #ExternalProject_Add_StepDependencies(${module} CONFIGURE_COMMAND fftw3f-dsp)
    # Configurations
    set(${lib-name-interface}_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${module}/${install_lib_dir})

    # Get External Project Include path
    ExternalProject_Get_Property(${module} SOURCE_DIR)
    ExternalProject_Get_Property(${module} BINARY_DIR)
    ExternalProject_Get_Property(${module} INSTALL_DIR)
    SET(${lib-name-interface}_INCLUDE_DIR "${INSTALL_DIR}/include")
    # Create include path
    file(MAKE_DIRECTORY ${${lib-name-interface}_INCLUDE_DIR})

    message("INSTALL DIR CUSTOM PRJ BIN DIR = " ${BINARY_DIR})
    # Create external library
    add_library(${lib-name-interface} ${build_type} IMPORTED)
    # set(${lib-name-interface}_LIBRARY "${${lib-name-interface}_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_${build_type}_LIBRARY_PREFIX}${lib-name-interface}${CMAKE_${build_type}_LIBRARY_SUFFIX}")
    set(${lib-name-interface}_LIBRARY "${${module}_INSTALL_DIR}/lib/${lib-name}")
    # Find paths and set dependencies
    add_dependencies(${lib-name-interface} ${module} ${dep_lib})

    message("LIB NAME EXTERNAl ==== " ${${lib-name-interface}_LIBRARY})
    # Set interface properties
    set_target_properties(${lib-name-interface} PROPERTIES IMPORTED_LOCATION ${${lib-name-interface}_LIBRARY})
    #include_directories(${${lib-name-interface}_INCLUDE_DIR})
    #set_target_properties(${lib-name-interface} PROPERTIES PUBLIC_HEADER ${${lib-name-interface}_INCLUDE_DIR})
    #set_target_properties(${lib-name-interface} PROPERTIES HEADER ${${lib-name-interface}_INCLUDE_DIR})
    target_include_directories(${lib-name-interface} INTERFACE ${${lib-name-interface}_INCLUDE_DIR})

endmacro(add_3rd_party_make)


#
#   Add external project.
#
#   \param name             Name of external project
#   \param path             Path to source directory
#   \param external         Name of the external target
#
macro(add_3rd_party_make lib-name-interface configure-cmd module lib-name dep_lib build_type)
    # Create external project
    set(${module}_SOURCE_DIR ${THIRD_PARTY_DIR}/${module})
    set(${module}_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${module})
    set(${module}_CMAKE_ARGS ${CMAKE_ARGS})
    set(${module}_INSTALL_DIR ${CMAKE_INSTALL_PREFIX})
    message("INSTALL DIR CUSTOM PRJ = " ${${module}_INSTALL_DIR})
    message(" SOURCE DIR CUSTOM PRJ = " ${${module}_SOURCE_DIR})
    message("    BIN DIR CUSTOM PRJ = " ${${module}_BINARY_DIR})
    ExternalProject_Add(${module}
        SOURCE_DIR "${${module}_SOURCE_DIR}"
        BINARY_DIR "${${module}_BINARY_DIR}"
        #BUILD_IN_SOURCE 1
        BUILD_ALWAYS 1
        PATCH_COMMAND ${CMAKE_SOURCE_DIR}/patches/patch-manager.sh ${module} "${${module}_SOURCE_DIR}"
        CONFIGURE_COMMAND "${configure-cmd}"
        DEPENDS ${dep_lib}
        BUILD_COMMAND make -j10
        INSTALL_DIR "${${module}_INSTALL_DIR}"
        INSTALL_COMMAND make install
    )

    ExternalProject_Add_Step(
        ${module} UNIX_CONFIGURE
        ALWAYS 1
        COMMENT "Copying source to build directory (make build) ${module}"
        DEPENDEES patch
        DEPENDERS configure
        COMMAND cp -ur "${${module}_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}"
    )
    
  #ExternalProject_Add_Step(
  #      ${module} UNIX_CONFIGURE
  #      ALWAYS 1
  #      COMMENT "ldconfig"
  #      DEPENDEES install
  #      COMMAND ldconfig
  #  )

    #ExternalProject_Add_StepDependencies(${module} CONFIGURE_COMMAND fftw3f-dsp)
    # Configurations
    set(${lib-name-interface}_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${module}/${install_lib_dir})

    # Get External Project Include path
    ExternalProject_Get_Property(${module} SOURCE_DIR)
    ExternalProject_Get_Property(${module} BINARY_DIR)
    ExternalProject_Get_Property(${module} INSTALL_DIR)
    SET(${lib-name-interface}_INCLUDE_DIR "${INSTALL_DIR}/include")
    # Create include path
    file(MAKE_DIRECTORY ${${lib-name-interface}_INCLUDE_DIR})

    message("INSTALL DIR CUSTOM PRJ BIN DIR = " ${BINARY_DIR})
    # Create external library
    add_library(${lib-name-interface} ${build_type} IMPORTED)
    # set(${lib-name-interface}_LIBRARY "${${lib-name-interface}_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_${build_type}_LIBRARY_PREFIX}${lib-name-interface}${CMAKE_${build_type}_LIBRARY_SUFFIX}")
    set(${lib-name-interface}_LIBRARY "${${module}_INSTALL_DIR}/lib/${lib-name}")
    # Find paths and set dependencies
    add_dependencies(${lib-name-interface} ${module} ${dep_lib})

    message("LIB NAME EXTERNAl ==== " ${${lib-name-interface}_LIBRARY})
    # Set interface properties
    set_target_properties(${lib-name-interface} PROPERTIES IMPORTED_LOCATION ${${lib-name-interface}_LIBRARY})
    #include_directories(${${lib-name-interface}_INCLUDE_DIR})
    #set_target_properties(${lib-name-interface} PROPERTIES PUBLIC_HEADER ${${lib-name-interface}_INCLUDE_DIR})
    #set_target_properties(${lib-name-interface} PROPERTIES HEADER ${${lib-name-interface}_INCLUDE_DIR})
    target_include_directories(${lib-name-interface} INTERFACE ${${lib-name-interface}_INCLUDE_DIR})

endmacro(add_3rd_party_make)

#
#   Add external project.
#
#   \param name             Name of external project
#   \param path             Path to source directory
#   \param external         Name of the external target
#
macro(add_external_project lib-name module build_type)
    # Create external project
    #set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/)
    string(REPLACE "/" "-" target ${module})
    message("PATH:- " ${THIRD_PARTY_DIR}/${module})
    set(${module}_SOURCE_DIR ${THIRD_PARTY_DIR}/${module})
    set(${module}_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${module})
    set(${module}_INSTALL_DIR ${CMAKE_INSTALL_PREFIX})
    # Set lib bin dir
    # parse if extra argument provided for lib install suffix (lib/lib64) <- this extra argument will force to use suffix instaead of using system lib suffix
    set(lib_suffix ${ARGN})
    list(LENGTH lib_suffix lib_suffix_len)
    if ( ${lib_suffix_len} GREATER 0 )
        set(install_lib_dir ${lib_suffix})
    else()
        set(install_lib_dir ${INSTALL_LIB_DIR})
    endif()
    set(${lib-name}_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${module}/${install_lib_dir})
    # set library <path/libname>
    set(${lib-name}_LIBRARY "${${lib-name}_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_${build_type}_LIBRARY_PREFIX}${lib-name}${CMAKE_${build_type}_LIBRARY_SUFFIX}")

    # Get CMAKE Args to External Project
    #set(${module}_CMAKE_ARGS ${CMAKE_ARGS})
    #string (REPLACE ";" " " ${module}_CMAKE_ARGS "${${module}_CMAKE_ARGS}")
    #message("CMAKE ARGS to ExternalProject  : " "${${module}_CMAKE_ARGS}")

    ExternalProject_Add(${target}
        SOURCE_DIR "${${module}_SOURCE_DIR}"
        BINARY_DIR "${${module}_BINARY_DIR}"
        BUILD_BYPRODUCTS "${${lib-name}_LIBRARY}"
        #CMAKE_ARGS "${${module}_CMAKE_ARGS}"
        BUILD_ALWAYS 1
        CMAKE_ARGS -DGTEST_FLAG=DISABLE
                   -DADDRESS_SANITIZER=${ADDRESS_SANITIZER}
                   -DMEMORY_SANITIZER=${MEMORY_SANITIZER}
                   -DUB_SANITIZER=${UB_SANITIZER}
                   -DINTEGER_SANITIZER=${INTEGER_SANITIZER}
                   -DIMPLICIT_CONVERSION_SANITIZER=${IMPLICIT_CONVERSION_SANITIZER}
                   -DNULLABILITY_SANITIZER=${NULLABILITY_SANITIZER}
                   -DTHREAD_SANITIZER=${THREAD_SANITIZER}
                   -DCFI_SANITIZER=${CFI_SANITIZER}
                   -DCMAKE_OVERRIDE_COMPILER_VER=${CMAKE_OVERRIDE_COMPILER_VER}
                   -DCMAKE_INSTALL_PREFIX:PATH=${${module}_INSTALL_DIR}
        INSTALL_DIR "${${module}_INSTALL_DIR}"
        INSTALL_COMMAND
    )

    # Configurations
    # Get External Project Include path
    ExternalProject_Get_Property(${target} SOURCE_DIR)
    ExternalProject_Get_Property(${target} INSTALL_DIR)
    SET(${lib-name}_INCLUDE_DIR "${INSTALL_DIR}/include")
    # Create include path
    file(MAKE_DIRECTORY ${${lib-name}_INCLUDE_DIR})

    # Create external library
    add_library(${lib-name} ${build_type} IMPORTED)
    # Find paths and set dependencies
    add_dependencies(${lib-name} ${target})

    # Set interface properties
    set_target_properties(${lib-name} PROPERTIES IMPORTED_LOCATION ${${lib-name}_LIBRARY})
    #include_directories(${${lib-name}_INCLUDE_DIR})
    #set_target_properties(${lib-name} PROPERTIES PUBLIC_HEADER ${${lib-name}_INCLUDE_DIR})
    #set_target_properties(${lib-name} PROPERTIES HEADER ${${lib-name}_INCLUDE_DIR})
    target_include_directories(${lib-name} INTERFACE ${${lib-name}_INCLUDE_DIR})

endmacro(add_external_project)

macro(add_external_project_dep lib-name module build_type dep_lib)
    # Create external project
    #set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/)
    string(REPLACE "/" "-" target ${module})
    message("PATH:- " ${THIRD_PARTY_DIR}/${module})
    set(${module}_SOURCE_DIR ${THIRD_PARTY_DIR}/${module})
    set(${module}_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${module})
    set(${module}_INSTALL_DIR ${CMAKE_INSTALL_PREFIX})
    # Set lib bin dir
    # parse if extra argument provided for lib install suffix (lib/lib64) <- this extra argument will force to use suffix instaead of using system lib suffix
    set(lib_suffix ${ARGN})
    list(LENGTH lib_suffix lib_suffix_len)
    if ( ${lib_suffix_len} GREATER 0 )
        set(install_lib_dir ${lib_suffix})
    else()
        set(install_lib_dir ${INSTALL_LIB_DIR})
    endif()
    set(${lib-name}_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${module}/${install_lib_dir})
    # set library <path/libname>
    set(${lib-name}_LIBRARY "${${lib-name}_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_${build_type}_LIBRARY_PREFIX}${lib-name}${CMAKE_${build_type}_LIBRARY_SUFFIX}")
    # Get CMAKE Args to External Project
    #set(${module}_CMAKE_ARGS ${CMAKE_ARGS})
    #string (REPLACE ";" " " ${module}_CMAKE_ARGS "${${module}_CMAKE_ARGS}")
    #message("CMAKE ARGS to ExternalProject  : " "${${module}_CMAKE_ARGS}")
    ExternalProject_Add(${target}
        SOURCE_DIR "${${module}_SOURCE_DIR}"
        BINARY_DIR "${${module}_BINARY_DIR}"
        BUILD_BYPRODUCTS "${${lib-name}_LIBRARY}"
        #CMAKE_ARGS "${${module}_CMAKE_ARGS}"
        BUILD_ALWAYS 1
        CMAKE_ARGS -DGTEST_FLAG=DISABLE
                   -DADDRESS_SANITIZER=${ADDRESS_SANITIZER}
                   -DMEMORY_SANITIZER=${MEMORY_SANITIZER}
                   -DUB_SANITIZER=${UB_SANITIZER}
                   -DINTEGER_SANITIZER=${INTEGER_SANITIZER}
                   -DIMPLICIT_CONVERSION_SANITIZER=${IMPLICIT_CONVERSION_SANITIZER}
                   -DNULLABILITY_SANITIZER=${NULLABILITY_SANITIZER}
                   -DTHREAD_SANITIZER=${THREAD_SANITIZER}
                   -DCFI_SANITIZER=${CFI_SANITIZER}
                   -DCMAKE_INSTALL_PREFIX:PATH=${${module}_INSTALL_DIR}
        DEPENDS ${dep_lib}
        INSTALL_DIR "${${module}_INSTALL_DIR}"
        INSTALL_COMMAND
    )
    # Configurations
    # Get External Project Include path
    ExternalProject_Get_Property(${target} SOURCE_DIR)
    ExternalProject_Get_Property(${target} INSTALL_DIR)
    SET(${lib-name}_INCLUDE_DIR "${INSTALL_DIR}/include")
    # Create include path
    file(MAKE_DIRECTORY ${${lib-name}_INCLUDE_DIR})
    # Create external library
    add_library(${lib-name} ${build_type} IMPORTED)
    # Find paths and set dependencies
    add_dependencies(${lib-name} ${target} ${dep_lib})
    # Set interface properties
    set_target_properties(${lib-name} PROPERTIES IMPORTED_LOCATION ${${lib-name}_LIBRARY})
    #include_directories(${${lib-name}_INCLUDE_DIR})
    #set_target_properties(${lib-name} PROPERTIES PUBLIC_HEADER ${${lib-name}_INCLUDE_DIR})
    #set_target_properties(${lib-name} PROPERTIES HEADER ${${lib-name}_INCLUDE_DIR})
    target_include_directories(${lib-name} INTERFACE ${${lib-name}_INCLUDE_DIR})
endmacro(add_external_project_dep)

#
#   Add external project.
#
#   \param name             Name of external project
#   \param path             Path to source directory
#   \param external         Name of the external target
#
macro(add_3rd_party_cmake lib-name module build_type)
    # Create external project
    #set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/)
    string(REPLACE "/" "-" target ${module})
    message("PATH:- " ${THIRD_PARTY_DIR}/${module})
    set(${module}_SOURCE_DIR ${THIRD_PARTY_DIR}/${module})
    set(${module}_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${module})
    set(${module}_INSTALL_DIR ${CMAKE_INSTALL_PREFIX})
    # Set lib bin dir
    # parse if extra argument provided for lib install suffix (lib/lib64) <- this extra argument will force to use suffix instaead of using system lib suffix
    set(lib_suffix ${ARGN})
    list(LENGTH lib_suffix lib_suffix_len)
    if ( ${lib_suffix_len} GREATER 0 )
        set(install_lib_dir ${lib_suffix})
    else()
        set(install_lib_dir ${INSTALL_LIB_DIR})
    endif()
    set(${lib-name}_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${module}/${install_lib_dir})
    # set library <path/libname>
    set(${lib-name}_LIBRARY "${${lib-name}_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_${build_type}_LIBRARY_PREFIX}${lib-name}${CMAKE_${build_type}_LIBRARY_SUFFIX}")

    # Get CMAKE Args to External Project
    #set(${module}_CMAKE_ARGS ${CMAKE_ARGS})
    #string (REPLACE ";" " " ${module}_CMAKE_ARGS "${${module}_CMAKE_ARGS}")
    #message("CMAKE ARGS to ExternalProject  : " "${${module}_CMAKE_ARGS}")

    ExternalProject_Add(${target}
        SOURCE_DIR "${${module}_SOURCE_DIR}"
        BINARY_DIR "${${module}_BINARY_DIR}"
        BUILD_BYPRODUCTS "${${lib-name}_LIBRARY}"
        #CMAKE_ARGS "${${module}_CMAKE_ARGS}"
        PATCH_COMMAND ${CMAKE_SOURCE_DIR}/patches/patch-manager.sh ${module} "${${module}_SOURCE_DIR}"
        BUILD_ALWAYS 1
        CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                   -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
                   -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
                   -DCMAKE_INSTALL_PREFIX:PATH=${${module}_INSTALL_DIR}
        INSTALL_DIR "${${module}_INSTALL_DIR}"
        INSTALL_COMMAND
    )

    # Configurations
    # Get External Project Include path
    ExternalProject_Get_Property(${target} SOURCE_DIR)
    ExternalProject_Get_Property(${target} INSTALL_DIR)
    SET(${lib-name}_INCLUDE_DIR "${INSTALL_DIR}/include")
    # Create include path
    file(MAKE_DIRECTORY ${${lib-name}_INCLUDE_DIR})

    # Create external library
    add_library(${lib-name} ${build_type} IMPORTED)
    # Find paths and set dependencies
    add_dependencies(${lib-name} ${target})

    # Set interface properties
    set_target_properties(${lib-name} PROPERTIES IMPORTED_LOCATION ${${lib-name}_LIBRARY})
    #include_directories(${${lib-name}_INCLUDE_DIR})
    #set_target_properties(${lib-name} PROPERTIES PUBLIC_HEADER ${${lib-name}_INCLUDE_DIR})
    #set_target_properties(${lib-name} PROPERTIES HEADER ${${lib-name}_INCLUDE_DIR})
    target_include_directories(${lib-name} INTERFACE ${${lib-name}_INCLUDE_DIR})

endmacro(add_3rd_party_cmake)



#
#   Add external target to external project.
#
#   \param name             Name of external project
#   \param includedir       Path to include directory
#   \param libdir           Path to library directory
#   \param build_type       Build type {STATIC, SHARED}
#   \param external         Name of the external target
#
macro(add_external_target name includedir libdir build_type external)
    # Configurations
    set(${name}_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${libdir}/lib)

    # Create external library
    add_library(${name} ${build_type} IMPORTED)
    set(${name}_LIBRARY "${${name}_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_${build_type}_LIBRARY_PREFIX}${name}${CMAKE_${build_type}_LIBRARY_SUFFIX}")
    # Find paths and set dependencies
    add_dependencies(${name} ${external})
    set(${name}_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${includedir}")

    # Set interface properties
    set_target_properties(${name} PROPERTIES IMPORTED_LOCATION ${${name}_LIBRARY})
    set_target_properties(${name} PROPERTIES INCLUDE_DIRECTORIES ${${name}_INCLUDE_DIR})
endmacro(add_external_target)

#
#   Add external target lib to external project.
#
#   \param name             Name of external project
#   \param libdir           Path to library directory
#   \param build_type       Build type {STATIC, SHARED}
#   \param external         Name of the external target
#
macro(add_external_target_lib name libdir build_type external)
    # Configurations
    set(${name}_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${libdir}/lib)

    # Get External Project Include path
    ExternalProject_Get_Property(${external} SOURCE_DIR)
    ExternalProject_Get_Property(${external} INSTALL_DIR)
    SET(${name}_INCLUDE_DIR "${INSTALL_DIR}/include")
    # Create include path
    file(MAKE_DIRECTORY ${${name}_INCLUDE_DIR})

    # Create external library
    add_library(${name} ${build_type} IMPORTED)
    set(${name}_LIBRARY "${${name}_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_${build_type}_LIBRARY_PREFIX}${name}${CMAKE_${build_type}_LIBRARY_SUFFIX}")
    # Find paths and set dependencies
    add_dependencies(${name} ${external})

    # Set interface properties
    set_target_properties(${name} PROPERTIES IMPORTED_LOCATION ${${name}_LIBRARY})
    #include_directories(${${name}_INCLUDE_DIR})
    #set_target_properties(${name} PROPERTIES PUBLIC_HEADER ${${name}_INCLUDE_DIR})
    #set_target_properties(${name} PROPERTIES HEADER ${${name}_INCLUDE_DIR})
    target_include_directories(${name} INTERFACE ${${name}_INCLUDE_DIR})
endmacro(add_external_target_lib)


