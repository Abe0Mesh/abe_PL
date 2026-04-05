#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>
// use 'cmake --build .' to build, it calls build/ folder and builds using it
// execute with './abe' as executble is in build folder and execuable is named abe

enum class TokenType{
    semicolon,
    digit,
    return_


};
struct Token{
        TokenType type;
        // optional is used to represent data that may or may not exist
        std::optional<std::string> value;

};

std::vector<Token> tokenize(const std::string& str) {
    std::vector<Token> tokens;
    std::string buffer;

    for (int i = 0; i < str.length(); i++) {
        char c = str.at(i);
        if(std::isalpha(c)) {
            buffer.push_back(c);
            i++;
            while (std::isalnum(str.at(i))){
                buffer.push_back(str.at(i));
                i++;
            }
            i--;

            if(buffer == "return") {
                // using the designated initializer here, allowing me to inilize only the token type field leaving other fields to default 
                tokens.push_back({.type = TokenType::return_});
                buffer.clear();
                continue;
            }

            else {
                std::cerr << "No return" << std::endl;
                exit(EXIT_FAILURE);
            }

        }
        // handling digits
        else if (std::isdigit(c)) {
            buffer.push_back(c);
            i++;
            while (std::isdigit(str.at(i))) {
                buffer.push_back(str.at(i));
                i++;
            }
            i--;
            tokens.push_back({.type = TokenType::digit, .value = buffer});
            buffer.clear();
        }
        else if (c == ';') {
            tokens.push_back({.type = TokenType::semicolon});
        }
        // handling whitespace
        else if (std::isspace(c)) {
            continue;
        }
        else {
            std::cerr << "code donst exist in abe" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    return tokens;
}
// study start
std::string tokens_to_asm(const std::vector<Token>& tokens) {
    std::stringstream output;
    output << ".global _start\n_start:\n";
    for (int i = 0; i < tokens.size(); i++) {
        const Token& token = tokens.at(i);
        if (token.type == TokenType::return_) { // if first token is return
            if(i + 1 <tokens.size() && tokens.at(i + 1).type == TokenType::digit) {
                if (i + 2 < tokens.size() && tokens.at(i+2).type == TokenType::semicolon) {
                    output << "    mov $60, %rax\n";
                    output << "    mov $" << tokens.at(i + 1).value.value() << ", %rdi\n";
                    output << "    syscall";
                }
            }
        }
    }
    return output.str();

}

// study end

// argc is for argument count and argv holds arguments from cmd line
int main(int argc, char* argv[]) {

    if (argc != 2) {
        // std cerr is used to output errors 
        std::cerr << "Incorrect usage. Instead do... " << std::endl;
        std::cerr << " abe <input.abe> " << std::endl; // assuming working in build/ terminal
        return EXIT_FAILURE; // fancy way to do return 1
    }

    std::string contents; // this string will hold source code from .abe file
    // we use a temp scope here so content stream n input get auto destroyed
    {   
        // below is a default construcor call but c++ allows use to call default cons w/o ()
        std::stringstream contents_stream; // sets up temp memory that we can read and write strings 
        std::fstream input(argv[1], std::ios::in); // opens file thats passed as second cmd line arg and sets it up for reading
        contents_stream << input.rdbuf(); // grab the entire file as a stream (thats currently in the read buffer)
        contents = contents_stream.str(); //convernt stream to string

    }

    // study start

    // std::cout << contents << std::endl; // output source code file (thats in contents)
    std::vector<Token> tokens = tokenize(contents);
    {
        std::fstream file("out.asm", std::ios::out);
        file << tokens_to_asm(tokens);        
    }

    system("nasm -felf64 out.asm");
    system("ld -o out out.o");

    // study end

    return EXIT_SUCCESS;
    

}



// todo:
// Figure out how to properly step and use debug mode
// review the codes I listened between study start and end to better understand the logic
