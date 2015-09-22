macro(gen_version_c)

	set(hfile ${CMAKE_CURRENT_BINARY_DIR}/version.h)
	set(cfile ${CMAKE_CURRENT_BINARY_DIR}/version.c)

	add_custom_command(
		OUTPUT ${hfile} ${cfile}
		COMMAND
			${CMAKE_CURRENT_SOURCE_DIR}/common/genversion.sh
		ARGS
			${CMAKE_CURRENT_BINARY_DIR}
		DEPENDS [!v][!e][!r]*.c [!v][!e][!r]*.h *.sh CMakeLists.txt
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMENT "Generating version.c, version.h"
	)

	set_directory_properties(ADDITIONAL_MAKE_CLEAN_FILES ${cfile} ${hfile})
	set_source_files_properties(${cfile} ${hfile} PROPERTIES GENERATED TRUE)

endmacro(gen_version_c)

macro(gen_resource_c file)

	set(hfile ${CMAKE_CURRENT_BINARY_DIR}/${file}.h)
	set(cfile ${CMAKE_CURRENT_BINARY_DIR}/${file}.c)

	add_custom_command(
		OUTPUT
			${hfile}
			${cfile}
		COMMAND
			${CMAKE_CURRENT_SOURCE_DIR}/common/make_c.sh
			${CMAKE_CURRENT_SOURCE_DIR}/${file}
			${hfile}
			${cfile}
		DEPENDS
			${CMAKE_CURRENT_SOURCE_DIR}/common/make_c.sh
			${CMAKE_CURRENT_SOURCE_DIR}/${file}
		WORKING_DIRECTORY
			${CMAKE_CURRENT_SOURCE_DIR}
		COMMENT
			"Generating ${file}.c, ${file}.h"
	)

	set_directory_properties(ADDITIONAL_MAKE_CLEAN_FILES ${cfile} ${hfile})
	set_source_files_properties(${cfile} ${hfile} PROPERTIES GENERATED TRUE)

endmacro(gen_resource_c)
