#pragma once
#include <dvigl/core.h>
#include <dvigl/node.h>

class Mesh;

class ModelNode : public Node {
public:
  ModelNode(char *content, int content_size, std::string format, float scale);
  ModelNode(int w, int h, std::string texture);
  void draw();
  void release();

private:
  std::vector<Mesh *> meshes;
};
