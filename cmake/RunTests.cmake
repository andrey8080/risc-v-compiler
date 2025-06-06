# RunTests.cmake - Simplified script for automated testing

set(COMPILER_EXECUTABLE "${CMAKE_BINARY_DIR}/bin/compiler")
set(TESTS_DIR "${CMAKE_SOURCE_DIR}/tests")
set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/test_outputs")

# Создаем директорию для выходных файлов тестов
file(MAKE_DIRECTORY ${OUTPUT_DIR})

message(STATUS "=== Educational Compiler Test Suite ===")
message(STATUS "Compiler: ${COMPILER_EXECUTABLE}")
message(STATUS "Tests directory: ${TESTS_DIR}")

# Получаем список всех .prog файлов
file(GLOB TEST_FILES "${TESTS_DIR}/*.prog")

set(TESTS_PASSED 0)
set(TESTS_FAILED 0)
set(FAILED_TESTS "")

foreach(INPUT_FILE ${TEST_FILES})
    get_filename_component(TEST_NAME ${INPUT_FILE} NAME)
    set(OUTPUT_FILE "${OUTPUT_DIR}/${TEST_NAME}.s")
    
    message(STATUS "Running test: ${TEST_NAME}")
    
    # Запускаем компилятор
    execute_process(
        COMMAND ${COMPILER_EXECUTABLE} ${INPUT_FILE} ${OUTPUT_FILE}
        RESULT_VARIABLE TEST_RESULT
        OUTPUT_VARIABLE TEST_OUTPUT
        ERROR_VARIABLE TEST_ERROR
        TIMEOUT 30
    )
    
    if(TEST_RESULT EQUAL 0)
        # Проверяем, что выходной файл создан
        if(EXISTS ${OUTPUT_FILE})
            message(STATUS "  ✅ PASS: Assembly generated successfully")
            math(EXPR TESTS_PASSED "${TESTS_PASSED} + 1")
        else()
            message(STATUS "  ❌ FAIL: No output file generated")
            math(EXPR TESTS_FAILED "${TESTS_FAILED} + 1")
            list(APPEND FAILED_TESTS ${TEST_NAME})
        endif()
    else()
        message(STATUS "  ❌ FAIL: Compiler returned error code ${TEST_RESULT}")
        if(TEST_ERROR)
            message(STATUS "     Error: ${TEST_ERROR}")
        endif()
        math(EXPR TESTS_FAILED "${TESTS_FAILED} + 1")
        list(APPEND FAILED_TESTS ${TEST_NAME})
    endif()
endforeach()

# Итоговый отчет
message(STATUS "")
message(STATUS "=== Test Results ===")
message(STATUS "Tests passed: ${TESTS_PASSED}")
message(STATUS "Tests failed: ${TESTS_FAILED}")
math(EXPR TOTAL_TESTS "${TESTS_PASSED} + ${TESTS_FAILED}")
message(STATUS "Total tests: ${TOTAL_TESTS}")

if(TESTS_FAILED GREATER 0)
    message(STATUS "")
    message(STATUS "Failed tests:")
    foreach(FAILED_TEST ${FAILED_TESTS})
        message(STATUS "  • ${FAILED_TEST}")
    endforeach()
    message(FATAL_ERROR "Some tests failed!")
else()
    message(STATUS "")
    message(STATUS "🎉 All tests passed!")
endif()
