cmake_minimum_required(VERSION 3.13)
project(COMP4981_asn2_test C)

set(CMAKE_C_STANDARD 99)

add_executable(COMP4981_asn2_test main.c server.c server.h client.c client.h files.c files.h message_queue.c message_queue.h semaphore.c semaphore.h threads.h main.h)