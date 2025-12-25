#ifndef _CACHE_DATA_
#define _CACHE_DATA_

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <vector>
#include <string>
//#include <sstream>

#ifndef OPERATION_STATUS
#define OPERATION_STATUS

#define OK  "\033[32m✔\033[0m"
#define ERR "\033[31m✗\033[0m"
#define WAR "\033[33m⚠\033[0m"

#endif

struct File {
    int fd;
    size_t sz;
    std::string name;
    std::string marker;
};

class CacheData {
public:

    ~CacheData();

    int create_cache_data(const char* marker);
    File find_cache_data(const char* marker);

    int write_cache_data(int fd, const char* data, size_t _size);
    int read_cache_data(int fd, char* data, size_t _size);

    std::vector<File>& get_cache_files();

private:
    std::vector<File> listTmpFiles;
};

#endif