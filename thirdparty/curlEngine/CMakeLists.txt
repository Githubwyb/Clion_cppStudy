cmake_minimum_required(VERSION 3.11)

project(curl_engine)

add_executable(
    ${PROJECT_NAME}
    main.cpp
)

target_link_libraries(
    ${PROJECT_NAME} PRIVATE
    libcurl
    skfApi
)
