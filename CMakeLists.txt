cmake_minimum_required(VERSION 2.8)

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
)

if(LIBXML2_FOUND)
	include_directories(${LIBXML2_INCLUDE_DIR})
	list(APPEND xml.c)
else()
	message("xml.c is OFF")
endif()

add_library(
	svc
	${SOURCES}
)

add_dependencies(svc mkresource mkversion)
