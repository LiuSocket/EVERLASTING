#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <string>
#include <vector>

class FileUtil {
public:
    static bool readFile(const std::string& path, std::vector<uint8_t>& outContent);
    static bool writeFile(const std::string& path, const std::vector<uint8_t>& content);
};

#endif // FILEUTIL_H