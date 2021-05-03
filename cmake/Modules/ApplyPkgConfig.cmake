# Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

#
# This function applies external (out of source tree) dependencies
# to given target. Arguments are:
#   TARGET - valid cmake target
#   PRIVACY - dependency can be inherited by dependent targets or not:
#     PUBLIC - this should be used by default, cause compile/link flags passing
#     PRIVATE - do not passes any settings to dependent targets,
#               may be usefull for static libraries from the inside of the project
# Argument ARGV2 and following are supposed to be names of checked pkg config
# packages. This function will use variables created by check_pkg_modules().
#  - ${DEP_NAME}_LIBRARIES
#  - ${DEP_NAME}_INCLUDE_DIRS
#  - ${DEP_NAME}_CFLAGS
#
FUNCTION(APPLY_PKG_CONFIG TARGET PRIVACY)
  MATH(EXPR DEST_INDEX "${ARGC}-1")
  FOREACH(I RANGE 2 ${DEST_INDEX})
    IF(NOT ${ARGV${I}}_FOUND)
      MESSAGE(FATAL_ERROR "Not found dependency - ${ARGV${I}}_FOUND")
    ENDIF(NOT ${ARGV${I}}_FOUND)
    TARGET_LINK_LIBRARIES(${TARGET} ${PRIVACY} "${${ARGV${I}}_LIBRARIES}")
    TARGET_INCLUDE_DIRECTORIES(${TARGET} ${PRIVACY} SYSTEM "${${ARGV${I}}_INCLUDE_DIRS}")
    STRING(REPLACE ";" " " CFLAGS_STR "${${ARGV${I}}_CFLAGS}")
    SET(CFLAGS_LIST ${CFLAGS_STR})
    SEPARATE_ARGUMENTS(CFLAGS_LIST)
    FOREACH(OPTION ${CFLAGS_LIST})
      TARGET_COMPILE_OPTIONS(${TARGET} ${PRIVACY} ${OPTION})
    ENDFOREACH(OPTION)
    SET_TARGET_PROPERTIES(${TARGET} PROPERTIES SKIP_BUILD_RPATH true)
  ENDFOREACH(I RANGE 2 ${DEST_INDEX})
ENDFUNCTION(APPLY_PKG_CONFIG TARGET PRIVACY)
