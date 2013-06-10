project( label )

set (	SRCS
	${GRAPHVIZSRC_DIRS}/lib/label/index.c
	${GRAPHVIZSRC_DIRS}/lib/label/node.c
	#${GRAPHVIZSRC_DIRS}/lib/label/nrtmain.c
	${GRAPHVIZSRC_DIRS}/lib/label/rectangle.c
	${GRAPHVIZSRC_DIRS}/lib/label/split.q.c
	${GRAPHVIZSRC_DIRS}/lib/label/xlabels.c
	)

include_directories (
	${GRAPHVIZSRC_DIRS}/lib/label
	)

if (WIN32)
	ADD_DEFINITIONS( -DHAVE_CONFIG_H -DHAVE_SETMODE -DMSWIN32)
else ()
	ADD_DEFINITIONS( -DHAVE_CONFIG_H -DHAVE_SETMODE )
endif ()

add_library ( label STATIC
			${SRCS}
			)
target_link_libraries ( label
	)
