#include <filesystem>
#include <iostream>
#include <sstream>
#include <iterator>

constexpr auto MSL_PREFIX_ERROR = "[ERROR] : ";

std::vector<std::string> collect_files(const std::string& path);

bool insert(const std::string& sfile);

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
    if (!insert(file_structure.str())) {
        std::cout << file_structure.str();
    }
    return 0;
}

std::vector<std::string> collect_files(const std::string& path) {
    std::vector<std::string> paths;
    if (exists(std::filesystem::absolute(path))) {
        for (const auto& entry: std::filesystem::directory_iterator(path)) {
            if (entry.is_directory()) {
                for (const auto &collect_file : collect_files(entry.path().c_str())) {
                    paths.push_back(collect_file);
                }
            } else {
                if (const auto path_string = entry.path().string();
                        path_string.find(".cpp") != std::string::npos ||
                        path_string.find(".c") != std::string::npos) {
                    paths.emplace_back(path_string);
                }
            }
        }
    }
    return paths;
}

bool insert(const std::string& sfile) {
    FILE* pipe = nullptr;
#if defined(__linux__)
    pipe = popen("xclip -selection clipboard", "w");
#elif defined(__APPLE__) || defined(__MACH__)
    pipe = popen("pbcopy", "w");
#endif
    if (pipe) {
        fwrite(sfile.c_str(), sizeof(char), sfile.size(), pipe);
        pclose(pipe);
        return true;
    }
    return false;
}
