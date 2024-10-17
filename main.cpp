#include <iostream>
#include <filesystem>
#include <vector>
#include <fstream>
#include <sys/stat.h>
#include <algorithm>

using namespace std; 

#define VERSION "v1.0.1"

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
    cout << "USAGE:" << endl;
    cout << "  tree [options] <path>" << endl;
    cout << endl;
    cout << "    Recursively print the files in the specified path." << endl;
    cout << endl;
    cout << "  Options:" << endl;
    cout << "    -h, --help              Show this help menu." << endl;
    cout << "    -v, --version           Show the version." << endl;
    cout << "    -d, --depth <number>    Display the tree up to the given depth." << endl;
    cout << "    -a, --all               Also display hidden files." << endl;
    cout << "OR" << endl;
    cout << "  tree exclude [options] <name>..." << endl;
    cout << endl;
    cout << "    Add the names provided to the excludes list. (~/.config/tree/excludes)" << endl;
    cout << "    Files with names that are inside the excludes list will never be" << endl;
    cout << "    printed, even if the '-a' option is given." << endl;
    cout << endl;
    cout << "  Options:" << endl;
    cout << "    -d, --delete            Delete the given names from the excludes list" << endl;
    cout << "                            instead of adding them." << endl;
    cout << "    -l, --list              List the items in the excludes list." << endl;
    cout << endl;
}

void error(const string msg, int code) {
    cerr << "\033[31mERROR: \033[0m" << msg << endl;
    exit(code);
}

struct File {
    string path;
    string name;
    bool is_directory;
};

enum class State {
    BCH,        // |-
    MID,        // | 
    END,        // '-
    EMP         //   
};

string state_to_string(State s) {
    string str;
    switch (s) {
        case State::BCH:
            str = "|--";
            break;
        case State::MID:
            str = "|  ";
            break;
        case State::END:
            str = "'--";
            break;
        case State::EMP:
            str = "   ";
            break;
        default:
            str = "   ";
            break;
    }
    return str;
}

vector<File> listdir(string path, bool get_hidden_files) {
    vector<File> files;
    try {
        for (const auto& entry : filesystem::directory_iterator(path)) {
            File file = {
                entry.path(),
                get_filename_from_path(entry.path()),
                entry.is_directory()
            };
            if (!get_hidden_files && file.name.substr(0, 1) == ".") {
                continue;
            }
            files.push_back(file);
        }
    } catch (const filesystem::filesystem_error& e) {
        error(e.what(), 1);
    }
    return files;
}

void tree(File file, int max_depth, vector<State> states, bool show_hidden_files, const vector<string>& exclude_list) {
    if (find(exclude_list.begin(), exclude_list.end(), file.name) != exclude_list.end()) {
        return;
    }
    for (int level=0; level<states.size(); ++level) {
        cout << state_to_string(states[level]);
    }
    if (file.is_directory) {
        cout << "+ \033[1;33m" << file.name << "\033[0m" << endl;
    } else {
        cout << "- " << file.name << endl;
    }
    if (max_depth > 0 && states.size() == max_depth) return;
    if (file.is_directory) {
        vector<File> children = listdir(file.path, show_hidden_files);
        int child_count = children.size();
        for (int i=0; i<child_count; ++i) {
            File child = children[i];
            vector<State> new_states;
            for (State s: states) {
                new_states.push_back(s);
            }
            int depth = new_states.size();
            if (new_states.size() > 0) {
                if      (new_states[depth-1] == State::BCH) new_states[depth-1] = State::MID;
                else if (new_states[depth-1] == State::END) new_states[depth-1] = State::EMP;
            }
            if      (i == child_count-1) new_states.push_back(State::END);
            else if (i <  child_count-1) new_states.push_back(State::BCH);
            tree(child, max_depth, new_states, show_hidden_files, exclude_list);
        }
    }
}

void save_exclude_list(vector<string> exclude_list) {
    string home_path = getenv("HOME");
    string excludes_path = home_path+"/.config/tree";
    if (stat(excludes_path.c_str(), nullptr) != -1) {
        if (mkdir(excludes_path.c_str(), 0755) == -1) {
            error("The path '"+excludes_path+"' could not be created.", 1);
        }
    }
    ofstream file(excludes_path+"/excludes");
    if (!file) {
        error("Could not open the file '"+excludes_path+"/excludes'", 1);
    }
    sort(exclude_list.begin(), exclude_list.end());
    for (const string& path : exclude_list) {
        file << path << endl;
    }
    file.close();
}

vector<string> load_exclude_list() {
    vector<string> exclude_list;
    string home_path = getenv("HOME");
    string excludes_path = home_path+"/.config/tree/excludes";
    ifstream file(excludes_path);
    if (!file) {
        error("Could not open the file '"+excludes_path+"/excludes'", 1);
    }
    string line;
    while (getline(file, line)) {
        exclude_list.push_back(line);
    }
    file.close();
    return exclude_list;
}

int main(int argc, char* argv[]) {

    if (argc == 1) {
        help();
        exit(0);
    } else if ((string)argv[1] == "exclude") {
        vector<string> exclude_list = load_exclude_list();

        bool will_delete = false;
        vector<string> file_names;
        for (int i=2; i<argc; ++i) {
            if ((string)argv[i] == "--delete" || (string)argv[i] == "-d") {
                will_delete = true;
            } else if ((string)argv[i] == "--list" || (string)argv[i] == "-l") {
                for (const string& name : exclude_list) {
                    cout << name << endl;
                }
                exit(0);
            } else {
                file_names.push_back((string)argv[i]);
            }
        }
        if (file_names.size() == 0) {
            error("Give path names after 'exclude'. Type 'tree --help' to see the usage.", 1);
        }

        if (will_delete) {
            for (const string& name : file_names) {
                exclude_list.erase(remove(exclude_list.begin(), exclude_list.end(), name), exclude_list.end());
            }
        } else {
            for (const string& name : file_names) {
                if (find(exclude_list.begin(), exclude_list.end(), name) == exclude_list.end()) {
                    exclude_list.push_back(name);
                }
            }
        }

        save_exclude_list(exclude_list);

    } else {

        int depth = 0;
        string file_path;
        bool show_hidden_files = false;

        for (int i=0; i<argc; ++i) {
            if ((string)argv[i] == "--depth" || (string)argv[i] == "-d") {
                if (argc == i+1) error("A number should come after '-d' tag.\nType 'tree --help' to see the usage.", 1);
                depth = atoi(argv[++i]);
            } else if ((string)argv[i] == "-a") {
                show_hidden_files = true;            
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

        const vector<string>& exclude_list = load_exclude_list();

        File file = {
            file_path,
            get_filename_from_path(file_path),
            filesystem::is_directory(file_path)
        };
        vector<State> states;
        tree(file, depth, states, show_hidden_files, exclude_list);
    }
    return 0;
}
