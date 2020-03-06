#pragma once
#include <dvigl/core.h>
#include <dvigl/node.h>

class Mesh;

class ModelNode : public Node
{
  public:
    ModelNode(char * content, int content_size);
    ModelNode(int w, int h);
    void draw();
    void release();

  private:
    Mesh * m;
};
