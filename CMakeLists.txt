# CMake requirements
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# libonsen-java-jni ------------------------------------------------------------
PROJECT(LibOnsenJavaJNI)

SET(
    libonsen-java-jni_SRCS
        src/archive_info.c
        src/file_utils.c
        src/onsen.c
        src/plugins.c
)

INCLUDE_DIRECTORIES("/usr/lib/jvm/java-7-openjdk-amd64/include")

# Usual stuff
ADD_DEFINITIONS(-W -Wall -ansi -pedantic)
ADD_LIBRARY(onsen-java-jni SHARED ${libonsen-java-jni_SRCS})
TARGET_LINK_LIBRARIES(onsen-java-jni onsen)
INSTALL(TARGETS onsen-java-jni DESTINATION lib)
