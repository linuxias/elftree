SET(APPROVAL_LIB lib)
ADD_LIBRARY(${APPROVAL_LIB} INTERFACE)
TARGET_INCLUDE_DIRECTORIES(${APPROVAL_LIB} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
