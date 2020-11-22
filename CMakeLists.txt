cmake_minimum_required(VERSION 3.10)

# set the project name
project(PDSBackup)


set(BOOST_ROOT "boost")
set(SERVER_DIR src/server)
set(SHARED_DIR src/shared)
set(CLIENT_DIR src/client)


# server
file(GLOB SERVER_SRC ${SERVER_DIR}/* ${SHARED_DIR}/*)
add_executable(server ${SERVER_SRC})

# client
file(GLOB CLIENT_SRC ${CLIENT_DIR}/* ${SHARED_DIR}/*)
add_executable(client ${CLIENT_SRC})

# add threads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_link_libraries(server PRIVATE Threads::Threads)
target_link_libraries(client PRIVATE Threads::Threads)
target_link_libraries (client ${Boost_LIBRARIES})


# add the executable