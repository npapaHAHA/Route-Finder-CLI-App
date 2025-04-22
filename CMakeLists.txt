cmake_minimum_required(VERSION 3.20)
project(Yandex_API LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)


add_executable(Yandex_API main.cpp
        FlightSearch.cpp
        FlightSearch.h)

if (WIN32)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif ()

if (POLICY CMP0141)

    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif ()


include(FetchContent)

FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git GIT_TAG 3b15fa82ea74739b574d705fea44959b58142eb8)

FetchContent_MakeAvailable(cpr)

FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
)

FetchContent_MakeAvailable(nlohmann_json)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)

target_link_libraries(${PROJECT_NAME} PRIVATE cpr::cpr)

target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)