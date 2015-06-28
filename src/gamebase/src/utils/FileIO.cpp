#include <stdafx.h>
#include <gamebase/utils/FileIO.h>
#include <gamebase/utils/Exception.h>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <fstream>
#include <sstream>

namespace fs = boost::filesystem;

namespace gamebase {

std::string loadTextFile(const std::string& name)
{
    std::ifstream file(name);
    if (!file.good())
        THROW_EX() << "Can't open file: " << name;
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

std::vector<char> loadBinaryFile(const std::string& name)
{
    std::ifstream file(name, std::ios_base::binary);
    if (!file.good())
        THROW_EX() << "Can't open file: " << name;
    file.seekg(0, std::ios_base::end);
    size_t fileSize = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios_base::beg);
    
    std::vector<char> result(fileSize);
    file.read(&result.front(), fileSize);
    return result;
}

bool fileExists(const fs::path& path)
{
    return fs::exists(path);
}

bool fileExists(const std::string& pathStr)
{
    return fileExists(boost::filesystem::path(pathStr));
}

FileDesc fileInfo(const fs::path& path)
{
    try {
        if (!fileExists(path))
            return FileDesc();
        std::string fileName = path.filename();
        std::string extension;
        FileDesc::Type type;
        if (fs::is_directory(path)) {
            type = FileDesc::Directory;
        } else {
            type = fs::is_regular_file(path) ? FileDesc::File : FileDesc::Other;
            extension = path.extension();
            if (!extension.empty() && extension[0] == '.')
                extension = extension.substr(1);
            fileName = fileName.substr(0, fileName.size() - extension.size() - 1);
        }
        return FileDesc(path.string(), fileName, extension, type);
    } catch (std::exception& ex) {
        THROW_EX() << "Can't get info about file: " << path.string() << ". Reason: " << ex.what();
    }
}

FileDesc fileInfo(const std::string& pathStr)
{
    return fileInfo(fs::path(pathStr));
}

std::vector<FileDesc> listFilesInDirectory(const std::string& pathStr)
{
    try {
        if (!fileExists(pathStr))
            THROW_EX() << pathStr << " doesn't exist";
        fs::path path(pathStr);
        if (!fs::is_directory(path))
            THROW_EX() << pathStr << " isn't directory";

        std::vector<FileDesc> result;
        fs::directory_iterator endIt;
        for (fs::directory_iterator dirIt(path); dirIt != endIt; ++dirIt)
            result.push_back(fileInfo(dirIt->path()));
        return result;
    } catch (std::exception& ex) {
        THROW_EX() << "Can't list files in directory: " << pathStr << ". Reason: " << ex.what();
    }
}

void renameFile(const std::string& oldPathStr, const std::string& newPathStr)
{
    try {
        if (!fileExists(oldPathStr))
            THROW_EX() << oldPathStr << " doesn't exist";
        if (fileExists(newPathStr))
            THROW_EX() << newPathStr << " already exists";
        fs::rename(fs::path(oldPathStr), fs::path(newPathStr));
    } catch (std::exception& ex) {
        THROW_EX() << "Can't rename file " << oldPathStr << " to " << newPathStr << ". Reason: " << ex.what();
    }
}

void removeFile(const std::string& pathStr)
{
    try {
        if (!fileExists(pathStr))
            THROW_EX() << pathStr << " doesn't exist";
        fs::remove(fs::path(pathStr));
    } catch (std::exception& ex) {
        THROW_EX() << "Can't remove file: " << pathStr << ". Reason: " << ex.what();
    }
}

bool isAbsolutePath(const std::string& pathStr)
{
    try {
        return fs::path(pathStr).is_complete();
    } catch (std::exception& ex) {
        THROW_EX() << "Can't determine path type: " << pathStr << ". Reason: " << ex.what();
    }
}

void createDir(const std::string& pathStr)
{
    try {
        fs::create_directory(fs::path(pathStr));
    } catch (std::exception& ex) {
        THROW_EX() << "Can't create directory: " << pathStr << ". Reason: " << ex.what();
    }
}

std::string makePathStr(
    const std::string& path,
    const std::string& fileName,
    const std::string& extension)
{
    std::ostringstream ss;
    ss << path;
    if (!path.empty() && path.back() != '\\')
        ss << '\\';
    ss << fileName;
    if (!extension.empty() && extension.front() != '.')
        ss << '.';
    ss << extension;
    return ss.str();
}

}
