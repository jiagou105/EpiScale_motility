
include_directories(${THRUST_INCLUDE_DIR}) #specifies path that compiler looks in while searching for header files
 
include_directories(${COMMON_INCLUDES})
add_library(SceInit STATIC
        CellInitHelper.h
        CellInitHelper.cpp
        Signal2D.h
		Signal2D.cpp
		Signal_Calculator.hpp
		Signal_Calculator.cpp
		Tissue.hpp
		Tissue.cpp
		Mesh.hpp
		Mesh.cpp
		Cell.hpp
		Cell.cpp
		common.cpp
		common.hpp
        ${COMMON_INCLUDES}/Point2D.cpp
        ${COMMON_INCLUDES}/Point2D.h
        ${COMMON_INCLUDES}/ConfigParser.h
        ${COMMON_INCLUDES}/ConfigParser.cpp
        ${COMMON_INCLUDES}/GeoVector.h
        ${COMMON_INCLUDES}/GeoVector.cpp
)

target_compile_features(SceInit PUBLIC cxx_std_11)