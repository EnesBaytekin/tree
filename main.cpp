#include <iostream>

#define VERSION "1.0.0"

using namespace std; 

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
    cout << "\033[31mERROR: \033[0m" << msg << endl;
    exit(code);
}

int main(int argc, char* argv[]) {

    if (argc == 1) {
        help();
        exit(0);
    }

    int depth = 0;

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
        }
    }

    return 0;
}