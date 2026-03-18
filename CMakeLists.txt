cmake_minimum_required(VERSION 3.19)
project(AventúriersDuRail CXX)

# CMakeLists.txt pour le projet "Aventuriers du Rail"
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# On définit une configuration de build par défaut (Debug) si aucune n'est spécifiée
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
        "Type de build : Debug | Release | RelWithDebInfo | MinSizeRel" FORCE)
endif()

# Options de compilation pour une stricte conformité et des warnings élevés
if(MSVC)
    add_compile_options(/W4 /WX /utf-8)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# On liste les sources manuellement (plus fiable que GLOB en CI)
set(LIB_SOURCES
    src/Joueur.cpp
    src/Partie.cpp
    src/Plateau.cpp
    src/Ticket.cpp
    src/Train.cpp
    src/Ville.cpp
    src/VoieFerree.cpp
)

add_library(aventuriers_lib STATIC ${LIB_SOURCES})
target_include_directories(aventuriers_lib PUBLIC include)

# L'exécutable principal
add_executable(aventuriers src/main.cpp)
target_link_libraries(aventuriers PRIVATE aventuriers_lib)

# Tests unitaires avec Google Test
enable_testing()

find_package(GTest QUIET)

if(GTest_FOUND)
    include(GoogleTest)

    file(GLOB TEST_SOURCES "tests/*.cpp")

    if(TEST_SOURCES)
        add_executable(run_tests ${TEST_SOURCES})

        target_include_directories(run_tests PRIVATE include)

        target_link_libraries(run_tests
            PRIVATE
                GTest::gtest_main
                aventuriers_lib
        )

        # Découverte automatique des tests (cross-platform)
        gtest_discover_tests(run_tests
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            PROPERTIES TIMEOUT 30
        )
    else()
        message(STATUS "Aucun fichier de test trouvé dans tests/")
    endif()
else()
    message(WARNING
        "Google Test non trouvé – les tests ne seront pas compilés.\n"
        "  Linux  : sudo apt install libgtest-dev\n"
        "  macOS  : brew install googletest\n"
        "  Windows: vcpkg install gtest"
    )
endif()

# Documentation avec Doxygen
find_package(Doxygen QUIET)

if(DOXYGEN_FOUND)
    set(DOXYGEN_IN  ${CMAKE_SOURCE_DIR}/Doxyfile)
    set(DOXYGEN_OUT ${CMAKE_BINARY_DIR}/Doxyfile.generated)

    # On injecte les chemins dans le Doxyfile via configure_file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Génération de la documentation Doxygen..."
        VERBATIM
    )

    message(STATUS "Doxygen trouvé – cible 'doc' disponible (cmake --build . --target doc)")
else()
    message(WARNING
        "Doxygen non trouvé – la cible 'doc' ne sera pas disponible.\n"
        "  Linux  : sudo apt install doxygen graphviz\n"
        "  macOS  : brew install doxygen graphviz\n"
        "  Windows: https://www.doxygen.nl/download.html"
    )
endif()