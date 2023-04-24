find_package(Qt6 COMPONENTS Core Mqtt Widgets)
find_package(Protobuf REQUIRED)

qt_standard_project_setup()

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS decoder/sparkplug_b.proto)
include_directories(${PROTOBUF_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set_property(SOURCE ${PROTO_SRCS} ${PROTO_HDRS} PROPERTY SKIP_AUTOMOC ON)
set_property(SOURCE ${PROTO_SRCS} ${PROTO_HDRS} PROPERTY SKIP_AUTOUIC ON)
set_property(SOURCE ${PROTO_SRCS} ${PROTO_HDRS} PROPERTY SKIP_AUTOGEN ON)

add_executable(qmqttviewer WIN32
    broker.h
    brokerdialog.h brokerdialog.cpp brokerdialog.ui
    brokermodel.h brokermodel.cpp
    main.cpp
    messagemodel.h messagemodel.cpp
    mqttpayload.h
    qmqttviewer.cpp qmqttviewer.h qmqttviewer.ui
    subscriptionmodel.h subscriptionmodel.cpp
    decoder/abstractmessagedecoder.h
    decoder/base64decoder.h decoder/base64decoder.cpp
    decoder/hexmessagedecoder.cpp decoder/hexmessagedecoder.h
    decoder/jsonprettymessagedecoder.h decoder/jsonprettymessagedecoder.cpp
    decoder/plainmessagedecoder.cpp decoder/plainmessagedecoder.h
    decoder/sparkplugmessagedecoder.cpp decoder/sparkplugmessagedecoder.h
    ${FORMS_HEADERS}
    ${PROTO_SRCS} ${PROTO_HDRS}
)

target_link_libraries(qmqttviewer
    PRIVATE
        Qt6::Core
        Qt6::Mqtt
        Qt6::Widgets
        ${PROTOBUF_LIBRARY}
)
