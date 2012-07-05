###
## Tag Information
###
set(HPCC_PACKAGE_NAME "hpccsystems-graphcontrol")
set(HPCC_VERSION ${HPCC_MAJOR}.${HPCC_MINOR}.${HPCC_POINT})
set(HPCC_STAGE_VERSION ${HPCC_SEQUENCE})
if ( NOT "${HPCC_MATURITY}" STREQUAL "release" )
    set(HPCC_STAGE_VERSION "${HPCC_STAGE_VERSION}${HPCC_MATURITY}")
	if ( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
		set( HPCC_STAGE_VERSION "${HPCC_STAGE_VERSION}-Debug" )
	endif ()
endif ()

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	set ( ARCH64BIT 1 )
else ()
	set ( ARCH64BIT 0 )
endif ()
message ("-- 64bit architecture is ${ARCH64BIT}")

set(HPCC_PACKAGE_FILE_NAME "${HPCC_PACKAGE_NAME}_${HPCC_PROJECT}-${HPCC_VERSION}-${HPCC_STAGE_VERSION}")
message ("-- Package File Name is ${HPCC_PACKAGE_FILE_NAME}")
###
