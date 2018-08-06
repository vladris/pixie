#include "pixie.h"
#include <fstream>
#include <iterator>

using namespace pixie;

std::vector<word_t> load_program(const std::string& file)
{
    std::vector<word_t> program;
    std::fstream fs;
    fs.open(file, std::fstream::in);

    while (!fs.eof())
    {
        word_t word;
        fs >> word;
        program.push_back(word);
    }

    fs.close();
    return program;
}

int main(int argc, const char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: pixie <program>" << std::endl;
        return 0;
    }
	
    auto program = load_program(argv[1]);

    vm_t vm;
    vm.connect_input([]() { word_t input; std::cin >> input; return input; });
    vm.connect_output([](word_t output) { std::cout << output; });

    vm.run(program.begin(), program.end());
	
    return 0;
}
