message(CMAKE_MAKE_PROGRAM: "${CMAKE_MAKE_PROGRAM}")
if(${CMAKE_MAKE_PROGRAM} MATCHES "xcodebuild")
	set(TOOLDIR ${CMAKE_BINARY_DIR}/bin/Debug)
else()
	set(TOOLDIR ${CMAKE_BINARY_DIR}/bin)
endif()

macro(find_gitrep gitdir tag)
	exec_program(git ${gitdir}
		ARGS describe --tags
		OUTPUT_VARIABLE tagname
		RETURN_VALUE retcode
	)

	if(NOT retcode EQUAL 0)
		message(FATAL_ERROR "Cannot get tag for git repository ${gitdir}. Error: ${retcode}.")
	endif()

	if(NOT tagname EQUAL ${tag})
		message(FATAL_ERROR "tag ${tag} is not found for git repository ${gitdir}.")
	endif()
endmacro(find_gitrep)

macro(gen_version_c)

	set(hfile ${CMAKE_CURRENT_BINARY_DIR}/version.h)
	set(cfile ${CMAKE_CURRENT_BINARY_DIR}/version.c)

	add_custom_command(
		OUTPUT ${hfile} ${cfile}
		COMMAND
			${TOOLDIR}/mkversion
		ARGS
			${CMAKE_CURRENT_BINARY_DIR}
		DEPENDS *.c *.h *.sh CMakeLists.txt
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMENT "Generating version.c, version.h"
	)

	set_directory_properties(ADDITIONAL_MAKE_CLEAN_FILES ${cfile} ${hfile})
	set_source_files_properties(${cfile} ${hfile} PROPERTIES GENERATED TRUE)

endmacro(gen_version_c)

macro(gen_resource_c file)

	set(hfile ${CMAKE_CURRENT_BINARY_DIR}/${file}.h)
	set(cfile ${CMAKE_CURRENT_BINARY_DIR}/${file}.c)

	get_filename_component(OUTDIR ${hfile} DIRECTORY)

	add_custom_command(
		OUTPUT
			${hfile}
			${cfile}
		COMMAND
			${TOOLDIR}/mkresource
		ARGS
			${CMAKE_CURRENT_SOURCE_DIR}/${file}
		DEPENDS
			${CMAKE_CURRENT_SOURCE_DIR}/${file}
		WORKING_DIRECTORY
			${OUTDIR}
		COMMENT
			"Generating ${file}.c, ${file}.h"
	)

	set_directory_properties(ADDITIONAL_MAKE_CLEAN_FILES ${cfile} ${hfile})
	set_source_files_properties(${cfile} ${hfile} PROPERTIES GENERATED TRUE)

endmacro(gen_resource_c)

macro(generate_resources src_list dst_list)

	set(_src_list "${src_list}")
	
	foreach(res ${_src_list})
		gen_resource_c(${res})
		set(c ${CMAKE_CURRENT_BINARY_DIR}/${res}.c)
		set(h ${CMAKE_CURRENT_BINARY_DIR}/${res}.h)
		set(${dst_list} ${${dst_list}} ${c} ${h})
		set_source_files_properties(${c} ${h} PROPERTIES GENERATED TRUE)

	endforeach()

endmacro(generate_resources)
