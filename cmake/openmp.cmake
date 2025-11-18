# --- OpenMP setup for PPC project ---

# Настройка флагов OpenMP для MSVC (кроме Clang-cl)
if(MSVC AND NOT (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
  set(OpenMP_C_FLAGS
      "/openmp:llvm"
      CACHE STRING "OpenMP C flags" FORCE)
  set(OpenMP_CXX_FLAGS
      "/openmp:llvm"
      CACHE STRING "OpenMP CXX flags" FORCE)
  # Pretend Clang-style OpenMP 5.0 support so CMake reports a newer version
  set(OpenMP_C_SPEC_DATE
      201811
      CACHE STRING "OpenMP C specification date" FORCE)
  set(OpenMP_CXX_SPEC_DATE
      201811
      CACHE STRING "OpenMP CXX specification date" FORCE)
endif()

# Функция для линковки с потоками (pthread / Windows threads)
function(ppc_link_threads exec_func_lib)
  find_package(Threads REQUIRED)
  target_link_libraries(${exec_func_lib} PUBLIC Threads::Threads)
endfunction()

# Функция для линковки с OpenMP
function(ppc_link_openmp exec_func_lib)
  if(MSVC AND NOT (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    # MSVC
    target_compile_options(${exec_func_lib} PUBLIC /openmp)
    find_package(OpenMP REQUIRED)
    target_link_libraries(${exec_func_lib} PUBLIC OpenMP::OpenMP_CXX)
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND MSVC)
    # Clang-cl на Windows
    target_compile_options(${exec_func_lib} PUBLIC /openmp)
    target_link_libraries(${exec_func_lib}
                          PUBLIC "C:/Program Files/LLVM/lib/libomp.lib")
  else()
    # Linux / Unix
    find_package(OpenMP REQUIRED)
    target_compile_options(${exec_func_lib} PUBLIC ${OpenMP_CXX_FLAGS})
    target_link_libraries(${exec_func_lib} PUBLIC OpenMP::OpenMP_CXX)
  endif()
endfunction()
