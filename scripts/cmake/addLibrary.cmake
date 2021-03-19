function(add_joyeer_library name)
    set(options STATIC SHARED)
    set(single_parameter_options)
    set(multi_parameter_options)
    cmake_parse_arguments(JRL 
                            "${options}"
                            "${single_parameter_options}"
                            "${multi_parameter_options}"
                            ${ARGN}
                            )
    set(JRL_SOURCES ${JRL_UNPARSED_ARGUMENTS})


    if(JRL_SHARED)
        set(libkind SHARED)
    elseif(JRL_STATIC)
        set(libkind STATIC)
    endif()

    add_library(${name} ${libkind} ${JRL_SOURCES})
    
endfunction(add_joyeer_library name)


# joyeer tools executable
function(add_joyeer_executable executable)
    set(options)
    set(single_parameter_options)
    set(multi_parameter_options)
    cmake_parse_arguments(JRT 
                            "${options}"
                            "${single_parameter_options}"
                            "${multi_parameter_options}"
                            ${ARGN}
                            )

    set(JRT_SOURCES ${JRT_UNPARSED_ARGUMENTS})

    add_executable(${executable} ${JRT_SOURCES})
    
endfunction(add_joyeer_executable)
