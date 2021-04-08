#pragma once

#include <QThread>
#include <QWindow>
#include <dvigl/core.h>


class WorkerThread : public QThread
{
    Q_OBJECT
    void run() override {
        dvigl_init();

    Uint32 id = dvigl_get_window_id();
    QWindow* window = QWindow::fromWinId(id);
    window->show();

        int res = dvigl_main();

        QString result;
        /* ... here is the expensive or blocking operation ... */
        emit resultReady(result);
    }
signals:
    void resultReady(const QString &s);
};
