#include "qmqttviewer.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication::setOrganizationName("QMqttViewer");
    QApplication::setApplicationName("QMqttViewer");

    QApplication app(argc, argv);

    QMqttViewer view;
    view.resize(800, 600);
    view.show();

    return QApplication::exec();
}
