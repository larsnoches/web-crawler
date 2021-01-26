set(SRC_LIST main.cpp inputstream.cpp outputstream.cpp socket.cpp httpclient.cpp httprequestheader.cpp httpresponseheader.cpp util.cpp)

add_executable(${PROJECT_NAME} ${SRC_LIST})

if(UNIX)
    target_link_libraries(${PROJECT_NAME})
else(UNIX)
    target_link_libraries(${PROJECT_NAME} ws2_32)
endif(UNIX)

