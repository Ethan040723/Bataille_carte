cmake_minimum_required(VERSION 3.20)
project(test_multi)

set(CMAKE_CXX_STANDARD 20)

# --- CONFIGURATION SFML ---
set(SFML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SFML-2.6.2/lib/cmake/SFML")
find_package(SFML COMPONENTS graphics window network system REQUIRED)

# --- OPTIONS POUR LE .EXE AUTONOME (IMPORTANT) ---
if(WIN32)
    # Cette ligne règle ton erreur "libgcc_s_seh-1.dll introuvable"
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
endif()

# --- SERVEUR ---
add_executable(serveur_exe server.cpp)
target_link_libraries(serveur_exe sfml-graphics sfml-window sfml-network sfml-system)

# --- CLIENT ---
add_executable(client_exe client.cpp client.h)
target_link_libraries(client_exe sfml-graphics sfml-window sfml-network sfml-system)

# --- COPIE AUTOMATIQUE DES DLL SFML ---
if(WIN32)
    # Copie les DLL dans le dossier du serveur
    add_custom_command(TARGET serveur_exe POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/SFML-2.6.2/bin"
            $<TARGET_FILE_DIR:serveur_exe>)

    # Copie les DLL dans le dossier du client (Décommenté et corrigé)
    add_custom_command(TARGET client_exe POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/SFML-2.6.2/bin"
            $<TARGET_FILE_DIR:client_exe>)
endif()