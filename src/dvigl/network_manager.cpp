#include <dvigl/network_manager.h>

NetworkMgr gNetworkMgr;

bool NetworkMgr::init() {
  if (SDLNet_Init() != 0) {
    LOG("SDLNet_Init: %s\n", SDLNet_GetError());
    return false;
  }
  return true;
}

void NetworkMgr::release() { SDLNet_Quit(); }
