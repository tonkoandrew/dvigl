#pragma once
#include <QWidget>
#include <QThread>
#include <dvigl/core.h>

class Worker : public QThread
{
    Q_OBJECT
    void run() override {
        QString result;
        printf("START WORKER\n");
        dvigl_init();
        dvigl_main();
        emit resultReady(result);
    }

signals:
    void resultReady(const QString &result);
public:
    Uint32 win_id;
};