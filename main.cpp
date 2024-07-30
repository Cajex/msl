#include <filesystem>
#include <iostream>
#include <sstream>
#include <iterator>
#include <stack>

constexpr auto MSL_PREFIX_ERROR = "[ERROR] : ";

std::vector<std::string> collect_files(const char *path);

std::vector<std::filesystem::path> find_project_option(const char* name);

bool display_files(const char *sfile, size_t len);

int main(const int argc, char* arguments[]) {
    if (argc != 2) {
        std::cerr << MSL_PREFIX_ERROR << "unable arguments!" << std::endl;
        return -1;
    }

    auto path = std::string(arguments[1]);
    if (path.find('/') == std::string::npos) {
        if (const auto option = find_project_option(arguments[1]); !option.empty()) {
            const auto& project = option[0];
            std::cout << "try project " << project.c_str() << "\n";
            path = project;
        }
    }

    if (!std::filesystem::exists(path)) {
        std::cerr << MSL_PREFIX_ERROR << "unable to locate the given path!" << std::endl;
        return -1;
    }

    std::ostringstream file_structure;
    file_structure << "files(";

    if (const auto files = std::make_unique<std::vector<std::string>>(collect_files(path.c_str())); !files->empty()) {
        for (auto item = files->begin(); item != std::prev(files->end()); ++item) {
            file_structure << "'" << *item << "'" << "," << "\n";
        }
        file_structure << "'" << files->back() << "'" << ")";
    }

    if (!display_files(file_structure.str().c_str(), file_structure.str().size())) {
        std::cout << file_structure.str();
    }
    return 0;
}

std::vector<std::string> collect_files(const char *path) {
    std::vector<std::string> paths;
    std::stack<std::string> directories;

    directories.emplace(path);

    while (!directories.empty()) {
        std::string current_dir = directories.top();
        directories.pop();

        if (std::filesystem::exists(current_dir) && std::filesystem::is_directory(current_dir)) {
            for (const auto& entry : std::filesystem::directory_iterator(current_dir)) {
                if (entry.is_directory()) {
                    directories.push(entry.path().string());
                } else {
                    if (const auto path_string = entry.path().string(); path_string.find(".cpp") != std::string::npos || path_string.find(".c") != std::string::npos) {
                        paths.emplace_back(path_string);
                    }
                }
            }
        }
    }

    return paths;
}

bool display_files(const char *sfile, const size_t len) {
    FILE* pipe = nullptr;
#if defined(__linux__)
    pipe = popen("xclip -selection clipboard", "w");
#elif defined(__APPLE__) || defined(__MACH__)
    pipe = popen("pbcopy", "w");
#endif

    if (pipe) {
        fwrite(sfile, sizeof(char), len, pipe);
        pclose(pipe);
        std::cout << "copied to the clipboard" << std::endl;
        return true;
    }

    return false;
}

std::vector<std::filesystem::path> find_project_option(const char* name) {
    std::vector<std::filesystem::path> directories;
    try {
        for (const auto& item : std::filesystem::recursive_directory_iterator(std::filesystem::path(std::getenv("HOME")).string(), std::filesystem::directory_options::skip_permission_denied)) {
            std::cout << item << "\n";
            if (item.is_directory() && item.path().filename() == name) {
                directories.emplace_back(item.path());
            }
        }
    } catch (...) {
    }
    return directories;
}