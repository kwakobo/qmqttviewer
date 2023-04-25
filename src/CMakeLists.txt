find_package(Qt6 COMPONENTS Core Mqtt Widgets)

qt_standard_project_setup()

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
    ${RESOURCES}
    ${FORMS_HEADERS}
)

target_link_libraries(qmqttviewer
    PUBLIC
        Qt6::Core
        Qt6::Mqtt
        Qt6::Widgets
)
