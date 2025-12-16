# 1. Configuración obligatoria y nombre del proyecto
cmake_minimum_required(VERSION 3.10)
project(mario_game CXX) # CXX indica que es un proyecto C++

# 2. Equivalente a CXXFLAGS = -I include
# Le dice a CMake dónde buscar los archivos .h
include_directories(include)

# 3. Equivalente a CXXFLAGS = -Wall
# Añade banderas de compilación para mostrar advertencias
add_compile_options(-Wall)

# 4. Equivalente a SRC = ...
# Definimos una variable llamada SOURCES con tus archivos
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Player.cpp
    src/Enemy.cpp
    src/Tile.cpp
    src/Camera.cpp
)

# 5. Equivalente a EXEC y reglas de compilación (%.o: %.cpp)
# Crea el ejecutable "mario_game" usando los archivos en SOURCES
add_executable(mario_game ${SOURCES})

# 6. Equivalente a LDFLAGS = -lmingw32 -lSDL2main -lSDL2
# Enlaza las librerías al ejecutable.
# Nota: CMake maneja las extensiones (.lib, .a, .dll) automáticamente.
target_link_libraries(mario_game mingw32 SDL2main SDL2)