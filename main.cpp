#include <iostream>
#include <filesystem>
#include <vector>

using namespace std; 

#define VERSION "1.0.0"

string get_filename_from_path(string path) {
    int index = path.rfind('/');
    return path.substr(index+1);
}

void version() {
    cout << "Version: " << VERSION << endl;
}

void help() {
    cout << "Usage: tree [OPTIONS] [PATH]" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "    -h, --help               Show this help menu." << endl;
    cout << "    -v, --version            Show the version." << endl;
    cout << "    -d, --depth <number>     Print tree with the given depth." << endl;
    cout << endl;
}

void error(const string msg, int code) {
    cerr << "\033[31mERROR: \033[0m" << msg << endl;
    exit(code);
}

typedef struct {
    string path;
    string name;
    bool is_directory;
} File;

vector<File> listdir(string path) {
    vector<File> files;
    try {
        for (const auto& entry : filesystem::directory_iterator(path)) {
            File file = {
                entry.path(),
                get_filename_from_path(entry.path()),
                entry.is_directory()
            };
            files.push_back(file);
        }
    } catch (const filesystem::filesystem_error& e) {
        error(e.what(), 1);
    }
    return files;
}

void tree(string path, int depth, int max_depth) {
    if (max_depth > 0 && depth == max_depth)
        return;
    vector<File> files = listdir(path);
    for (auto& file : files) {
        for (int i=0; i<depth; ++i)
            cout << ". ";
        cout << file.name << endl;
        if (file.is_directory) {
            tree(file.path, depth+1, max_depth);
        }
    }
}

int main(int argc, char* argv[]) {

    if (argc == 1) {
        help();
        exit(0);
    }

    int depth = 0;
    string file_name;

    for (int i=0; i<argc; ++i) {
        if ((string)argv[i] == "--depth" || (string)argv[i] == "-d") {
            if (argc == i+1) error("A number should come after '-d' tag.\nType 'tree --help' to see the usage.", 1);
            depth = atoi(argv[++i]);
        } else if ((string)argv[i] == "--version" || (string)argv[i] == "-v") {
            version();
            exit(0);
        } else if ((string)argv[i] == "--help" || (string)argv[i] == "-h") {
            help();
            exit(0);
        } else {
            file_name = argv[i];
        }
    }

    tree(file_name, 0, depth);

    return 0;
}
