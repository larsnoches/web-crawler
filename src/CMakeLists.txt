set(SRC_LIST main.cpp inputstream.cpp outputstream.cpp socket.cpp httpclient.cpp httprequestheader.cpp httpresponseheader.cpp util.cpp)

add_executable(${PROJECT_NAME} ${SRC_LIST})

#find_package(OpenSSL)
include(FindOpenSSL)

if(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_DL_LIBS} OpenSSL::SSL OpenSSL::Crypto)
else(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32 ${CMAKE_DL_LIBS} OpenSSL::SSL OpenSSL::Crypto)
endif(UNIX)

