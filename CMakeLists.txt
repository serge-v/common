cmake_minimum_required(VERSION 2.8)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(LibXml2)
find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)

include_directories(${OPENSSL_INCLUDE_DIR})

add_executable(mkresource mkresource.c)
add_executable(mkversion mkversion.c)

list(APPEND SOURCES 
	socket.c
	http.c
	struct.c
	crypt.c
	net.c
	regexp.c
	log.c
	timer.c
	arrays.c
	parrays.c
	bst.c
	fs.c
	xml.c
)

add_library(
	svc
	${SOURCES}
)

add_dependencies(svc mkresource mkversion)
