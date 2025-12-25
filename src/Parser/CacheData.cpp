#include "Parser/CacheData.hpp"

CacheData::~CacheData() {

    for (auto next_fd : listTmpFiles) {
            unlink(next_fd.name.c_str());
            close(next_fd.fd);
        }

    listTmpFiles.clear();
}

int CacheData::create_cache_data(const char* marker) {
    char filename[] = "/tmp/dataXXXXXX";
    int fd = mkstemp(filename);

    if (fd == -1)
        return -1;

    listTmpFiles.push_back({ fd, 0, std::string(filename), std::string(marker) });
    return fd;
}

File CacheData::find_cache_data(const char* marker) {
    for (auto i : listTmpFiles)
        if (i.marker == marker)
            return i;
    return {-1,0,"",""};
}

int CacheData::write_cache_data(int fd, const char* data, size_t _size) {
    return write(fd, data, _size);
}

int CacheData::read_cache_data(int fd, char* data, size_t _size) {
    return read(fd, data, _size);
}

std::vector<File>& CacheData::get_cache_files() {
    return listTmpFiles;
}