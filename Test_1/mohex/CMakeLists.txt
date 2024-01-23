file(GLOB mohex_SRC *.cpp *.hpp *.h)

add_library(mohex STATIC ${mohex_SRC})

target_link_libraries(mohex
    Qt${QT_VERSION_MAJOR}::Widgets
)
