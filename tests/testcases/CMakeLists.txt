file(GLOB SRCS *.cpp)

ADD_EXECUTABLE(testcases ${SRCS})

TARGET_LINK_LIBRARIES(testcases
)

add_test(NAME testcases
         COMMAND testcases)
