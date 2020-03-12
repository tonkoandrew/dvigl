#pragma once
#include <dvigl/core.h>

class NetworkMgr : public Singleton<NetworkMgr> {
public:
  bool init();
  void release();
};