#include <iostream>
#include <cstring>

#define VERSION "1.0.0"

void version() {
    std::cout << "Version: " << VERSION << std::endl;
}

void help() {
    std::cout << "Usage: tree [OPTIONS] [PATH]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "    -h, --help               Show this help menu." << std::endl;
    std::cout << "    -v, --version            Show the version." << std::endl;
    std::cout << "    -d, --depth <number>     Print tree with the given depth." << std::endl;
    std::cout << std::endl;
}

void error(const char* msg, int code) {
    std::cout << "\033[31mERROR: \033[0m" << msg << std::endl;
    exit(code);
}

int main(int argc, char* argv[]) {

    if (argc == 1) {
        help();
        exit(0);
    }

    int depth = 0;

    for (int i=0; i<argc; ++i) {
        if (strcmp(argv[i], "--depth") == 0 || strcmp(argv[i], "-d") == 0) {
            if (argc == i+1) error("A number should come after '-d' tag.\nType 'tree --help' to see the usage.", 1);
            depth = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0) {
            version();
            exit(0);
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            help();
            exit(0);
        }
    }

    return 0;
}
