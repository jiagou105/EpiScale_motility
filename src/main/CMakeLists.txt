include_directories(${GOOGLETEST_DIR}/include/ ${COMMON_INCLUDES} ${GPU_SOURCE_DIR} ${CPU_SOURCE_DIR})
include_directories(${THRUST_INCLUDE_DIR}) #specifies path that compiler looks in while searching for header files


include_directories(${COMMON_CUDA_INCLUDES}) #specifies path that compiler looks in while searching for header files



set(DISK_M_PROJECT_NAME runDiscSimulation_M)
MESSAGE("Now building " ${DISK_M_PROJECT_NAME})
file(GLOB SRC_FILES ./discMain_M.cu)
#MESSAGE("Compiling these files: "${SRC_FILES})
add_executable(${DISK_M_PROJECT_NAME} ${SRC_FILES})
#add_dependencies(${DISK_M_PROJECT_NAME} gtest)
#add_dependencies(${DISK_M_PROJECT_NAME} gtest_main)
add_dependencies(${DISK_M_PROJECT_NAME} SceGPU)

if(NOT WIN32)

        #target_link_libraries(${DISK_M_PROJECT_NAME} pthread)
        target_link_libraries(${DISK_M_PROJECT_NAME} SceGPU)
        target_link_libraries(${DISK_M_PROJECT_NAME} SceInit)
        target_link_libraries(${DISK_M_PROJECT_NAME} SceUtils)
        #target_link_libraries(${DISK_M_PROJECT_NAME} SceUtilsGPU)

 #       target_link_libraries(${DISK_M_PROJECT_NAME}
 #               ${GOOGLETEST_DIR}/libgtest.a
 #              ${GOOGLETEST_DIR}/libgtest_main.a
 #      )

							
endif()

