#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

int pc = -1;

int num_symb = 0;
std::map<std::string, std::string> symb;

int showHelp() {
    std::cout << "Use \".\\assembler [args...]\"" << std::endl
              << "Arguments are:" << std::endl
              << "--input/-i [filename]" << std::endl
              << "    Input source code from [filename]" << std::endl
              << "--output/-o [filename]" << std::endl
              << "    Output binary code to [filename]" << std::endl
              << "--help/-h" << std::endl
              << "    Show this help menu" << std::endl
              << "The source code read by default is in the same directory named \"code.txt\" text file" << std::endl
              << "The binary code write by default is in the same directory named \"bin_code.o\" text file" << std::endl;
    return 0;
}
std::vector<std::string> RCode = {
    "mov", "add", "sub", "mul", "ifsm", "set"
};
int isRCode(std::string code) {
    for (auto r_code : RCode) {
        if (code == r_code) return 1;
    }
    return 0;
}
int lenNumber(std::string code) {
    if      (isRCode(code))     return 2;
    else if (code == "jmp")     return 1;
    else if (code == "input")   return 1;
    else if (code == "output")  return 1;
    else if (code == "load")    return 2;
    else if (code == "store")   return 2;
    else {
        std::cout << "Fail to encode " << code << " on pc = " << pc << "." << std::endl;
        exit(-1);
    }
}
std::string encode(const std::string &code) {
    if      (code == "mov")     return "00000000";
    else if (code == "add")     return "00000001";
    else if (code == "sub")     return "00000010";
    else if (code == "mul")     return "00000011";
    else if (code == "ifsm")    return "00000100";
    else if (code == "set")     return "00000101";
    else if (code == "jmp")     return "11111011";
    else if (code == "input")   return "11111100";
    else if (code == "output")  return "11111101";
    else if (code == "load")    return "11111110";
    else if (code == "store")   return "11111111";
    else {
        std::cout << "Fail to encode " << code << " on pc = " << pc << "." << std::endl;
        exit(0);
    }
}
std::string toBin(const int &len, const std::string &number) {
    int temp = 0;
    for (int i = 0; i < number.size(); ++i) {
        if (!isdigit(number[i])) continue;
        temp = temp * 10 + number[i] - '0';
    }
    std::string result;
    while (temp) {
        int t = temp % 2;
        temp /= 2;
        result.push_back(t + '0');
    }
    while (result.size() < len) result.push_back('0');
    std::reverse(result.begin(), result.end());
    return result;
}
std::string enRCode(const std::string &code, const std::string number[]) {
    std::string result;
    result = result + toBin(4, number[0]);
    result = result + toBin(4, number[1]);
    return result;
}
std::string enMCode(const std::string &code, const std::string number[]) {
    if (code == "store") return toBin(8, number[0]);
    else if (code == "load") return toBin(8, number[1]);
    else if (code == "input") return toBin(8, number[0]);
    else if (code == "output") return toBin(8, number[0]);
    else if (code == "jmp") {
        if (symb.find(number[0]) != symb.end()) return toBin(8, symb[number[0]]);
        else {
            std::cout << "Cannot find symbol \'" << number[0] << "\' at pc = " << pc << "." << std::endl;
            exit(0);
        }
    }
    else {
        std::cout << "Fail to encode on pc = " << pc << "." << std::endl;
        exit(0);
    }
}
int isSymb(std::string code) {
    if (code.back() == ':') return 1;
    return 0;
}
void enSymb(std::string code) {
    // delete ':'
    code.pop_back();
    symb[code] = std::to_string(pc);
}
int noNumber(std::string code) {
    if (isSymb(code)) return 1;
    return 0;
}

int main(int argc, const char *argv[]) {
    std::string file_name("code.txt"), output_name("bin_code.o");
    for (int i = 1; i < argc; i += 2) {
        std::string arg(argv[i]);
        if      (arg == "--output" || arg == "-o") output_name = std::string(argv[i + 1]);
        else if (arg == "--input"  || arg == "-i") file_name = std::string(argv[i + 1]);
        else if (arg == "--help"   || arg == "-h") return showHelp();
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
    std::ofstream ofs(output_name, std::ofstream::out);
    if (!ofs.is_open()) {
        std::cout << "Fail to open output file." << std::endl;
        return 0;
    }
    std::string code, number[2], bin_code, bin_number;
    while (ifs >> code) {
        ++pc;
        if (isSymb(code)) {
            enSymb(code);
            continue;
        }
        else bin_code = encode(code);
        ofs << bin_code;
        if (!noNumber(code)) {
            for (int i = 0; i < lenNumber(code); ++i) {
                ifs >> number[i];
            }
            if (isRCode(code)) bin_number = enRCode(code, number);
            else bin_number = enMCode(code, number);
            ofs << ' ' << bin_number;
        }
        ofs << std::endl;
    }
    return 0;
}