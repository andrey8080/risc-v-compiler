cmake_minimum_required(VERSION 3.16)
project(EducationalCompiler VERSION 1.0 LANGUAGES CXX)

# Настройки C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Поиск зависимостей
find_package(FLEX 2.6 REQUIRED)
find_package(BISON 3.0 REQUIRED)

# Настройка выходных директорий
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Создаем директорию для сгенерированных файлов
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/generated)

# Генерация парсера и лексера
BISON_TARGET(Parser 
    src/parser.y 
    ${CMAKE_BINARY_DIR}/generated/parser.cpp 
    DEFINES_FILE ${CMAKE_BINARY_DIR}/generated/parser.tab.h
    VERBOSE
)

FLEX_TARGET(Lexer 
    src/lexer.l 
    ${CMAKE_BINARY_DIR}/generated/lexer.cpp
)

ADD_FLEX_BISON_DEPENDENCY(Lexer Parser)

# Настройка include директорий
include_directories(
    ${CMAKE_BINARY_DIR}/generated
    ${CMAKE_SOURCE_DIR}/src
)

# Основной исполняемый файл
add_executable(compiler
    src/main.cpp
    src/ast.cpp
    src/codegen.cpp
    ${FLEX_Lexer_OUTPUTS}
    ${BISON_Parser_OUTPUTS}
)

# Флаги компиляции
target_compile_options(compiler PRIVATE
    $<$<CONFIG:Debug>:-g -O0 -Wall -Wextra>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
)

# =============================================================================
# CUSTOM TARGETS (PHONY COMMANDS)
# =============================================================================

# Очистка временных файлов в корне проекта
add_custom_target(clean-temp
    COMMAND ${CMAKE_COMMAND} -E remove -f
        ${CMAKE_SOURCE_DIR}/*.s
        ${CMAKE_SOURCE_DIR}/*.o
        ${CMAKE_SOURCE_DIR}/*.c
        ${CMAKE_SOURCE_DIR}/*.h
        ${CMAKE_SOURCE_DIR}/lex.yy.c
        ${CMAKE_SOURCE_DIR}/parser.tab.*
        ${CMAKE_SOURCE_DIR}/debug_parser.y
        ${CMAKE_SOURCE_DIR}/simple_*
        ${CMAKE_SOURCE_DIR}/temp_file.*
        ${CMAKE_SOURCE_DIR}/src/*.o
        ${CMAKE_SOURCE_DIR}/src/*.c
        ${CMAKE_SOURCE_DIR}/src/*.h
        ${CMAKE_SOURCE_DIR}/src/lex.yy.c
        ${CMAKE_SOURCE_DIR}/src/parser.tab.*
        ${CMAKE_SOURCE_DIR}/src/compiler
    COMMENT "Cleaning temporary files from project root and src/"
)

# Полная очистка включая build директорию  
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}
    DEPENDS clean-temp
    COMMENT "Full clean: removing build directory and temp files"
)

# Организация тестовых файлов
add_custom_target(organize-tests
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/tests
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/test*.prog ${CMAKE_SOURCE_DIR}/tests/ || true
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_SOURCE_DIR}/test*.prog
    COMMENT "Moving test files to tests/ directory"
)

# Запуск всех тестов
add_custom_target(test-all
    COMMAND ${CMAKE_SOURCE_DIR}/run_tests.sh
    DEPENDS compiler
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running all compiler tests"
)

# Сборка в режиме отладки
add_custom_target(debug
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config Debug
    COMMENT "Building in Debug mode"
)

# Сборка в режиме релиза
add_custom_target(release
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config Release  
    COMMENT "Building in Release mode"
)

# Установка компилятора
install(TARGETS compiler
    RUNTIME DESTINATION bin
    COMPONENT compiler
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/tests/
    DESTINATION share/educational-compiler/tests
    COMPONENT examples
    FILES_MATCHING PATTERN "*.prog"
)
