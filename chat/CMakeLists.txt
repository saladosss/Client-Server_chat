get_filename_component(chat_proto "../proto/chat.proto" ABSOLUTE)
get_filename_component(chat_proto_path "${chat_proto}" PATH)

set(chat_proto_srs "${CMAKE_CURRENT_BINARY_DIR}/chat.pb.cc")
set(chat_proto_hdr "${CMAKE_CURRENT_BINARY_DIR}/chat.pb.h")
set(chat_grpc_src "${CMAKE_CURRENT_BINARY_DIR}/chat.grpc.pb.cc")
set(chat_grpc_hdr "${CMAKE_CURRENT_BINARY_DIR}/chat.grpc.pb.h")
add_custom_command(
        OUTPUT "${chat_proto_srs}" "${chat_proto_hdr}" "${chat_grpc_src}" "${chat_grpc_hdr}"
        COMMAND ${_PROTOBUF_PROTOC}
        ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}" --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${chat_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${chat_proto}"
        DEPENDS "${chat_proto}"
)

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_library(chat_grpc_proto
        ${chat_proto_srs}
        ${chat_proto_hdr}
        ${chat_grpc_src}
        ${chat_grpc_hdr}
)
target_link_libraries(chat_grpc_proto
        ${_PROTOBUF_LIBPROTOBUF}
        ${_REFLECTION}
        ${_GRPC_GRPCPP})

foreach(_target client server)
    add_executable(${_target} "${_target}.cc")
    target_link_libraries(${_target}
            chat_grpc_proto
            ${_PROTOBUF_LIBPROTOBUF}
            ${_REFLECTION}
            ${_GRPC_GRPCPP})
endforeach()