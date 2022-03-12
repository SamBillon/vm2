#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

int pc = -1;
int ok = 1;
int reg[16] = {0};
int mem[256] = {0};

int file_output = 0;
std::ofstream ofs;

int store_memory = 0;
std::string memory_file_name;


int showHelp() {
    std::cout << "Use \".\\vm [args...]\"" << std::endl
              << "Arguments are:" << std::endl
              << "--output/-o [filename]" << std::endl
              << "    Output the running results of the program to [filename]" << std::endl
              << "--load/-l [filename]" << std::endl
              << "    Load the memory of program from [filename] before running" << std::endl
              << "--store/-o [filename]" << std::endl
              << "    Store the memory of program to [filename] after running" << std::endl
              << "--code/-c [filename]" << std::endl
              << "    Read binaries from [filename]" << std::endl
              << "--debug/-d" << std::endl
              << "    Debug mode" << std::endl
              << "--help/-h" << std::endl
              << "    Show this help menu" << std::endl
              << "The binary program file read by default is in the same directory named \"bin_code.o\" text file" << std::endl;
    return 0;
}


int __LOCALDEBUG__ = 0;
std::string antiAssembler(const std::string &code) {
    if      (code == "00000000") return "mov";
    else if (code == "00000001") return "add";
    else if (code == "00000010") return "sub";
    else if (code == "00000011") return "mul";
    else if (code == "00000100") return "ifsm";
    else if (code == "00000101") return "set";
    else if (code == "11111011") return "jmp";
    else if (code == "11111100") return "input";
    else if (code == "11111101") return "output";
    else if (code == "11111110") return "load";
    else if (code == "11111111") return "store";
    else {
        std::cout << "Fail to anti-assembler " << code << " on pc = " << pc << "." << std::endl;
        exit(-1);
    }
}
void showDebugInfo(const std::string &code, const int opt_number[]) {
    if (file_output) ofs << "DEBUG Info:" << std::endl
                          << "pc = " << pc << "." << std::endl
                          << "\'ok\' flag is " << ok << "." << std::endl
                          << "Now is running: " << std::endl
                          << antiAssembler(code) << ' ' << opt_number[0] << ' ' << opt_number[1] << std::endl;
    else std::cout << "DEBUG Info:" << std::endl
                   << "pc = " << pc << "." << std::endl
                   << "\'ok\' flag is " << ok << "." << std::endl
                   << "Now is running: " << std::endl
                   << antiAssembler(code) << ' ' << opt_number[0] << ' ' << opt_number[1] << std::endl;
}
void showDebugLine(const std::string &code, const int opt_number[]) {
    if (file_output) ofs << std::endl << std::endl;
    else std::cout << std::endl << std::endl;
}






int getInput() {
    int x;
    std::cin >> x;
    return x;
}
void output(int x) {
    if (file_output) ofs << x << std::endl;
    else std::cout << x << std::endl;
}



std::vector<std::string> RCode = {
//  mov         add         sub         mul         ifsm        set
    "00000000", "00000001", "00000010", "00000011", "00000100", "00000101"
};
int isRCode(std::string code) {
    for (auto r_code : RCode) {
        if (code == r_code) return 1;
    }
    return 0;
}
void deRCode(const std::string &number, int opt_number[]) {
    int result0 = 0, result1 = 0;
    for (int i = 0; i <= 3; ++i) result0 = result0 * 2 + number[i] - '0';
    for (int i = 4; i <= 7; ++i) result1 = result1 * 2 + number[i] - '0';
    opt_number[0] = result0;
    opt_number[1] = result1;
}
void deMCode(const std::string number, int opt_number[]) {
    int result = 0;
    for (int i = 0; i <= 7; ++i) result = result * 2 + number[i] - '0';
    opt_number[0] = result;
}
void runCode(const std::string code, const int opt_number[]) {
    if (!ok) ok = 1;
    else if (code == "00000000") reg[opt_number[0]] = reg[opt_number[1]];           // mov
    else if (code == "00000001") reg[opt_number[0]] += reg[opt_number[1]];          // add
    else if (code == "00000010") reg[opt_number[0]] -= reg[opt_number[1]];          // sub
    else if (code == "00000011") reg[opt_number[0]] *= reg[opt_number[1]];          // mul
    else if (code == "00000100") ok = reg[opt_number[0]] < reg[opt_number[1]];      // ifsm
    else if (code == "00000101") reg[opt_number[0]] = opt_number[1];                // set
    else if (code == "11111011") pc = opt_number[0] - 1;                            // jmp
    else if (code == "11111100") reg[opt_number[0]] = getInput();                   // input
    else if (code == "11111101") output(reg[opt_number[0]]);                        // output
    else if (code == "11111110") reg[0] = mem[opt_number[0]];                       // load
    else if (code == "11111111") mem[opt_number[0]] = reg[0];                       // store
    else {
        std::cout << "Fail to execute " << code << " on pc = " << pc << "." << std::endl;
        exit(-1);
    }
}


void loadMemory(const std::string &memory_name) {
    std::ifstream ifs_mem(memory_name, std::ifstream::in);
    int temp;
    int t = 0;
    while (ifs_mem >> temp) {
        mem[t++] = temp;
        if (t == 256) break;
    }
}
void setStoreMemory(const std::string &memory_name) {
    store_memory = 1;
    memory_file_name = memory_name;
}
void setOutput(const std::string &file_name, std::string &output_name) {
    output_name = file_name;
}
void storeMemory(const std::string &memory_name) {
    std::ofstream ofs_mem(memory_name, std::ofstream::out);
    for (int i = 0; i <= 255; ++i) ofs_mem << mem[i] << std::endl;
}
void setDebug(const int& x) {
    __LOCALDEBUG__ = 1;
}


std::vector<std::string> code;
std::vector<std::string> number;

int main(int argc, const char *argv[]) {
    std::string file_name("bin_code.o"), output_name;
    for (int i = 1; i < argc; i += 2) {
        std::string arg(argv[i]);
        if      (arg == "--help"    || arg == "-h") return showHelp();
        else if (arg == "--debug"   || arg == "-d") setDebug(--i);
        else if ((arg == "--output" || arg == "-o") && (i != argc - 1)) setOutput(std::string(argv[i + 1]), output_name);
        else if ((arg == "--store"  || arg == "-s") && (i != argc - 1)) setStoreMemory(std::string(argv[i + 1]));
        else if ((arg == "--code"   || arg == "-c") && (i != argc - 1)) file_name = std::string(argv[i + 1]);
        else if ((arg == "--load"   || arg == "-l") && (i != argc - 1)) loadMemory(std::string(argv[i + 1]));
        else {
            std::cout << "Wrong argument: \"" << arg << "\"." << std::endl;
            return 0;
        }
    }

    std::ifstream ifs(file_name, std::ifstream::in);
    if (!ifs.is_open()) {
        std::cout << "Fail to open source code." << std::endl;
        return 0;
    }

    if (output_name.size()) {
        ofs = std::ofstream(output_name, std::ofstream::out);
        file_output = 1;
        if (!ofs.is_open()) {
            std::cout << "Fail to open target output file." << std::endl
                      << "May be you can try \".\\vm --help\" or \".\\vm -h\" for help" << std::endl;
            return 0;
        }
    }

    std::string _code, _number;
    int opt_number[2] = {0, 0};
    while (ifs >> _code >> _number) {
        code.push_back(_code);
        number.push_back(_number);
    }

    while (++pc < code.size()) {
        _code = code[pc];
        _number = number[pc];
        if (isRCode(_code)) deRCode(_number, opt_number);
        else deMCode(_number, opt_number);

        if (__LOCALDEBUG__) showDebugInfo(_code, opt_number);

        runCode(_code, opt_number);

        if (__LOCALDEBUG__) showDebugLine(_code, opt_number);
    }
    if (store_memory) storeMemory(memory_file_name);
    return 0;
}