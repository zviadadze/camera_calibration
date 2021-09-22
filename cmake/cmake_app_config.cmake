function(create_configuration_files)
    set(options)
    set(one_value_args 
        TEMPLATE_SOURCE_PATH 
        CONFIGURATION_PATH 
        )
    set(multi_value_args)

    cmake_parse_arguments(
        ARG 
        "${options}" 
        "${one_value_args}"
        "${multi_value_args}" 
        ${ARGN}
        )

    if(NOT EXISTS ${ARG_TEMPLATE_SOURCE_PATH})
        message(FATAL_ERROR "The template data path \"${ARG_TEMPLATE_SOURCE_PATH}\" is not found")
    endif()

    file(GLOB_RECURSE 
        src_conf_files
        RELATIVE ${ARG_TEMPLATE_SOURCE_PATH}
        "${ARG_TEMPLATE_SOURCE_PATH}/*"
        )

    if (src_conf_files)
        file(MAKE_DIRECTORY ${ARG_CONFIGURATION_PATH})

        foreach (src_conf_file ${src_conf_files})
            set(src_file ${ARG_TEMPLATE_SOURCE_PATH}/${src_conf_file})
            set(dest_file ${ARG_CONFIGURATION_PATH}/${src_conf_file})
            get_filename_component(dest_path ${dest_file} DIRECTORY)

            string(REGEX MATCH ".+\.in$" matched ${src_conf_file})
            if (NOT matched)
                file(COPY ${src_file} DESTINATION ${dest_path})
            else()
                string(FIND ${src_file} ".in" templ_pos REVERSE)
                if (NOT (templ_pos EQUAL -1))
                    string(SUBSTRING ${src_file} 0 ${templ_pos} orig_src_file)
                    set(orig_src_file "${orig_src_file}") 

                    configure_file(${src_file} ${orig_src_file})

                    file(MAKE_DIRECTORY ${dest_path})
                    file(COPY ${orig_src_file} DESTINATION ${dest_path})
                    file(REMOVE ${orig_src_file})
                endif()
            endif()
        endforeach()
    endif()

    message(STATUS "Configuration data files have been generated to ${ARG_CONFIGURATION_PATH}")
endfunction()

function(generate_application_config)
    set(options)
    set(one_value_args 
        NAME 
        TEMPLATE_SOURCE_PATH 
        CONFIGURATION_PATH 
        READONLY_DATA_PATH 
        MODIFIABLE_DATA_PATH 
        EXECUTABLE_PATH
        LIBRARY_PATH 
        TEMPORARY_DATA_PATH
        )
    set(multi_value_args 
        PATH_VARIABLES
        )

    cmake_parse_arguments(
        ARG 
        "${options}" 
        "${one_value_args}"
        "${multi_value_args}" 
        ${ARGN}
        )

    if (ARG_NAME)
        set(CORE_NAME ${ARG_NAME})
    else()
        message(FATAL_ERROR "The configuration name is empty")
    endif()

    if (NOT ARG_TEMPLATE_SOURCE_PATH)
        message(FATAL_ERROR "The configuration template path is empty")
    endif()
    
    if (ARG_CONFIGURATION_PATH)
        set(CONFIGURATION_PATH ${ARG_CONFIGURATION_PATH})
    else()
        message(FATAL_ERROR "The configuration path is empty")
    endif()
    
    if (ARG_READONLY_DATA_PATH)
        set(READONLY_DATA_PATH ${ARG_READONLY_DATA_PATH})
    else()
        message(FATAL_ERROR "The read only data path is empty")
    endif()

    if (ARG_MODIFIABLE_DATA_PATH)
        set(MODIFIABLE_DATA_PATH ${ARG_MODIFIABLE_DATA_PATH})
    else()
        message(FATAL_ERROR "The modifiable path is empty")
    endif()
    
    if (ARG_EXECUTABLE_PATH)
        set(EXECUTABLE_PATH ${ARG_EXECUTABLE_PATH})
    else()
        message(FATAL_ERROR "The executable binary path is empty")
    endif()
    
    if (ARG_LIBRARY_PATH)
        set(LIBRARY_PATH ${ARG_LIBRARY_PATH})
    else()
        message(FATAL_ERROR "The library path is empty")
    endif()
    
    if (ARG_TEMPORARY_DATA_PATH)
        set(TEMPORARY_DATA_PATH ${ARG_TEMPORARY_DATA_PATH})
    else()
        set(TEMPORARY_DATA_PATH "/tmp")
    endif()

    set(path_var)
    foreach(path_arg ${ARG_PATH_VARIABLES})
        if (path_var)
            set(${path_var} ${path_arg})
            set(path_var)
        else()
            set(path_var ${path_arg})
        endif()
    endforeach()

    create_configuration_files(
        TEMPLATE_SOURCE_PATH 
            ${ARG_TEMPLATE_SOURCE_PATH}/etc
        CONFIGURATION_PATH
            ${ARG_CONFIGURATION_PATH}
        )

    create_configuration_files(
        TEMPLATE_SOURCE_PATH 
            ${ARG_TEMPLATE_SOURCE_PATH}/var
        CONFIGURATION_PATH
            ${ARG_MODIFIABLE_DATA_PATH}
        )
endfunction()

