#include "FileUtil.h" // 确保包含了 FileUtil 的声明
#include <fstream>
#include <vector>

bool FileUtil::readFile(const std::string& path, std::vector<uint8_t>& outContent) {
    std::ifstream in(path, std::ios::binary | std::ios::ate);
    if (!in) return false;
    std::streamsize size = in.tellg();
    in.seekg(0, std::ios::beg); // 确保文件指针回到文件开头
    outContent.resize(static_cast<size_t>(size)); // 确保 outContent 已正确声明并使用
    if (size > 0 && !in.read(reinterpret_cast<char*>(outContent.data()), size)) {
        return false;
    }
    return true;
}

bool FileUtil::writeFile(const std::string& path, const std::vector<uint8_t>& content) {
    std::ofstream out(path, std::ios::binary);
    if (!out) return false;
    out.write(reinterpret_cast<const char*>(content.data()), content.size());
    return out.good();
}