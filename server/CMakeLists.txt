cmake_minimum_required(VERSION 3.12)
project(MyServer)

set(CMAKE_CXX_STANDARD 11)

# Utwórz plik wykonywalny
add_executable(server server.cpp handleDB.cpp)

# Dodaj bibliotekę pthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(server PRIVATE Threads::Threads)

# Dodaj bibliotekę SQLite
find_package(SQLite3 REQUIRED)
target_link_libraries(server PRIVATE SQLite::SQLite3)