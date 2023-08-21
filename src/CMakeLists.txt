find_package(Qt6 COMPONENTS Core Mqtt Widgets)
find_package(Protobuf REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS handler/sparkplug_b.proto)
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
    messageitemdelegate.h messageitemdelegate.cpp
    messagemodel.h messagemodel.cpp
    mqttpayload.h
    qmqttviewer.cpp qmqttviewer.h qmqttviewer.ui
    subscriptionitemdelegate.h subscriptionitemdelegate.cpp
    subscriptionmodel.h subscriptionmodel.cpp
    topicmodel.h topicmodel.cpp
    handler/abstractmessagehandler.h
    handler/base64handler.h handler/base64handler.cpp
    handler/hexmessagehandler.cpp handler/hexmessagehandler.h
    handler/jsonprettymessagehandler.h handler/jsonprettymessagehandler.cpp
    handler/plainmessagehandler.cpp handler/plainmessagehandler.h
    handler/sparkplugmessagehandler.cpp handler/sparkplugmessagehandler.h
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
