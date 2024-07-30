#include <filesystem>
#include <iostream>
#include <sstream>
#include <iterator>
#include <stack>

constexpr auto MSL_PREFIX_ERROR = "[ERROR] : ";

std::vector<std::string> collect_files(const std::string& path);

bool display_files(const std::string& sfile);

int main(const int argc, char* arguments[]) {
    if (argc != 2) {
        std::cerr << MSL_PREFIX_ERROR << "unable arguments!" << std::endl;
        return -1;
    }

    const auto path = arguments[1];
    if (!std::filesystem::exists(path)) {
        std::cerr << MSL_PREFIX_ERROR << "unable to locate the given path!" << std::endl;
        return -1;
    }

    std::ostringstream file_structure;
    file_structure << "files(";

    if (const auto files = std::make_unique<std::vector<std::string>>(collect_files(path)); !files->empty()) {
        for (auto item = files->begin(); item != std::prev(files->end()); ++item) {
            file_structure << "'" << *item << "'" << "," << "\n";
        }
        file_structure << "'" << files->back() << "'" << ")";
    }

    if (!display_files(file_structure.str())) {
        std::cout << file_structure.str();
    }
    return 0;
}

std::vector<std::string> collect_files(const std::string& path) {
    std::vector<std::string> paths;
    std::stack<std::string> directories;

    directories.push(path);

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


bool display_files(const std::string& sfile) {
    FILE* pipe = nullptr;
#if defined(__linux__)
    pipe = popen("xclip -selection clipboard", "w");
#elif defined(__APPLE__) || defined(__MACH__)
    pipe = popen("pbcopy", "w");
#endif

    if (pipe) {
        fwrite(sfile.c_str(), sizeof(char), sfile.size(), pipe);
        pclose(pipe);
        std::cout << "copied to the clipboard" << std::endl;
        return true;
    }

    return false;
}
