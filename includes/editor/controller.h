#pragma once

#include <QThread>
#include <editor/worker.h>
#include <dvigl/core.h>


class Controller : public QObject
{
    Q_OBJECT
    Worker worker;
public:
    Controller() {
        printf("START Controller\n");
        worker.start();
    }
    ~Controller() {
        worker.quit();
        worker.wait();
    }
    Uint32 get_window_id();
public slots:
    void handleResults(const QString &);
signals:
    void operate(const QString &);
};
