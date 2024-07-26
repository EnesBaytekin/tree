#include <iostream>
#include <filesystem>
#include <vector>

using namespace std; 

#define VERSION "1.0.0"

string get_filename_from_path(string path) {
    if (path == "/") return path;
    if (path.size() > 1 && path.at(path.size()-1) == '/') path = path.substr(0, path.size()-1);
    int index = path.rfind('/');
    if (index == string::npos) return path;
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

typedef enum {
    BCH,        // |-
    MID,        // | 
    END,        // '-
    EMP         //   
} state;

string state_to_string(state s) {
    string str;
    switch (s) {
        case BCH:
            str = "|--";
            break;
        case MID:
            str = "|  ";
            break;
        case END:
            str = "'--";
            break;
        case EMP:
            str = "   ";
            break;
        default:
            str = "   ";
            break;
    }
    return str;
}

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

void tree(File file, int max_depth, vector<state> states) {
    for (int level=0; level<states.size(); ++level) {
        cout << state_to_string(states[level]);
    }
    string symbol = file.is_directory ? "+" : "-";
    cout << symbol << " " << file.name << endl;
    if (max_depth > 0 && states.size() == max_depth) return;
    if (file.is_directory) {
        vector<File> children = listdir(file.path);
        int child_count = children.size();
        for (int i=0; i<child_count; ++i) {
            File child = children[i];
            vector<state> new_states;
            for (state s: states) {
                new_states.push_back(s);
            }
            int depth = new_states.size();
            if (new_states.size() > 0) {
                if      (new_states[depth-1] == BCH) new_states[depth-1] = MID;
                else if (new_states[depth-1] == END) new_states[depth-1] = EMP;
            }
            if      (i == child_count-1) new_states.push_back(END);
            else if (i <  child_count-1) new_states.push_back(BCH);
            tree(child, max_depth, new_states);
        }
    }
}

int main(int argc, char* argv[]) {

    if (argc == 1) {
        help();
        exit(0);
    }

    int depth = 0;
    string file_path;

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
            file_path = argv[i];
        }
    }

    File file = {
        file_path,
        get_filename_from_path(file_path),
        filesystem::is_directory(file_path)
    };
    vector<state> states;
    tree(file, depth, states);
    return 0;
}
