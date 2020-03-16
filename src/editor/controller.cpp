#include <dvigl/core.h>

#include <editor/controller.h>

void Controller::handleResults(const QString &s) { printf("handleResults\n"); }

Uint32 Controller::get_window_id() {
  printf("WINID: %d\n", worker.win_id);
  return worker.win_id;
}