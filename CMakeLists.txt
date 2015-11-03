cmake_minimum_required(VERSION 3.2)

add_library(
	svc
	socket.c socket.h
	http.c http.h
	struct.c struct.h
	crypt.c crypt.h
	net.c net.h
)
