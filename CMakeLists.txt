cmake_minimum_required(VERSION 3.2)

find_package(LibXml2 REQUIRED)
find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)

include_directories(${LIBXML2_INCLUDE_DIR})
include_directories(${OPENSSL_INCLUDE_DIR})

add_executable(mkresource mkresource.c)

add_library(
	svc
	socket.c
	http.c
	struct.c
	crypt.c
	net.c
	xml.c
)

add_dependencies(svc mkresource)
