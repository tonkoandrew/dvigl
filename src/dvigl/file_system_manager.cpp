#include <dvigl/file_system_manager.h>

FileSystemMgr gFileSystemMgr;

bool FileSystemMgr::init() { return true; }

char *FileSystemMgr::get_content(std::string file_name) {
  char *buffer = NULL;
  Sint64 bufferLength = 0;

  SDL_RWops *rw = SDL_RWFromFile(file_name.c_str(), "rb");
  if (rw != NULL) {
    /* Seek to 0 bytes from the end of the file */
    bufferLength = SDL_RWsize(rw);
    if (bufferLength == 0) {
      LOG("File is empty: %s\n", file_name.c_str());
    } else {
      SDL_RWseek(rw, 0, RW_SEEK_SET);
      buffer = (char *)malloc(bufferLength + 1);
      if (buffer != NULL) {
        SDL_RWread(rw, buffer, bufferLength, 1);
        buffer[bufferLength] = '\0';
      } else {
        LOG("MEMORY ALLOCATION ERROR for file %s\n", file_name.c_str());
        return NULL;
      }
    }
    SDL_RWclose(rw);
    return buffer;
  } else {
    LOG("ERROR LOADING FILE %s - file not opened\n", file_name.c_str());
    return NULL;
  }
}

int FileSystemMgr::get_size(std::string file_name) {
  int size = 0;
  SDL_RWops *rw = SDL_RWFromFile(file_name.c_str(), "rb");
  if (rw != NULL) {
    size = SDL_RWsize(rw);
    SDL_RWclose(rw);
  } else {
    LOG("File not exist: %s\n", file_name.c_str());
  }
  return size;
}

void FileSystemMgr::release() {}