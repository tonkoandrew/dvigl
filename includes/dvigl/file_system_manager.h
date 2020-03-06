#pragma once
#include <dvigl/core.h>

class FileSystemMgr : public Singleton<FileSystemMgr>
{
  public:
    bool init();
    void release();
    char * get_content(std::string file_name);
    int get_size(std::string file_name);
};