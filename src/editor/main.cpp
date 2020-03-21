#include "mainwindow.h"
#include <QApplication>
// #include <dvigl/core.h>

int main(int argc, char *argv[]) {


//   const struct aiScene *scene;
//   const struct aiScene *scene2;

//   int flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals |
//               aiProcess_LimitBoneWeights | aiProcess_FindInvalidData |
//               aiProcess_SplitByBoneCount | aiProcess_FlipUVs |
//               // aiProcess_JoinIdenticalVertices | aiProcess_GlobalScale| 0;
//               aiProcess_JoinIdenticalVertices | 0;

// float scale = 1.0f; 
// // aiPropertyStore* store = aiCreatePropertyStore();
// // aiSetImportPropertyFloat(store, AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, scale);

// // scene = aiImportFile("../res/models/bob.dae", flags);
// // scene = aiImportFile("../res/models/boblampclean.md5mesh", flags);
// scene = aiImportFile(argv[1], flags);

// // aiReleasePropertyStore(store);


//   if (!scene) {
//     LOG("ERROR LOADING MODEL\n");
//     return -1;
//   }
//   LOG("IMPORTED %d MESHES\n", scene->mNumMeshes);
//   LOG("IMPORTED %d ANIMATIONS\n", scene->mNumAnimations);

// LOG("CONVERT FROM %s\n", argv[1]);
// LOG("CONVERT TO %s\n", argv[2]);
// aiExportScene(scene, "collada", argv[2], 0);

// return 0;
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
  }
