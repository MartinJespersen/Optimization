#include <iostream>
#include <fstream>
#include <deque>
#include "util.hpp"

std::string CharToString(char c){
    std::string output = std::string(1, c);
    return output;
}


int Lexer(char c, std::ifstream& file, std::deque<std::string>& tokenDeque){
    if(std::isspace(static_cast<unsigned char>(c)))
        return 1;
        
    if(c == '{' || c == ':' || c == '}'|| c == '['|| c == ']' || c == ','){
        tokenDeque.push_back(CharToString(c));
    }
    else if (c == '"'){
        std::string keyStr = "";
        tokenDeque.push_back(CharToString('"'));
        while(file.get(c))
        {
            if(c == '"')
                break;
            keyStr += c;
        }

        tokenDeque.push_back(keyStr);
        tokenDeque.push_back(CharToString('"'));
    }
    else if (c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || 
                c == '5' || c == '6' || c == '7' || c == '8' || c == '9' || c == '-'){
        std::string keyStr = CharToString(c);
        bool dotAvailable = false;

        while (file.get(c)){
            if(std::isspace(static_cast<unsigned char>(c)))
                continue;
            
            if(c == '.' && dotAvailable){

                std::cerr << "Float cannot contain multiple dot character\n";
                return -1;
            }
            else if(c == '.'){
                dotAvailable = true;
            }
            else if (c == '}' || c == ']' || c == ','){
                break;
            }
            else if (!(c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || 
                    c == '5' || c == '6' || c == '7' || c == '8' || c == '9')){
                std::cerr << "Float includes characters other than numbers\n";
                return -1;

            }

            keyStr += c;
        }

        tokenDeque.push_back(keyStr);
        tokenDeque.push_back(CharToString(c));
    }
    else
    {
        std::cerr << "expected the char value '{'\n";
        return -1;
    }
    return 1;
}

int StringParser(std::deque<std::string>& tokenDeque, std::string& str){
    std::string cur_str = tokenDeque.front();
    tokenDeque.pop_front();
    if (cur_str == "\""){
        std::cerr << "Key cannot be an empty string\n";
        return -1;
    }
    while(cur_str != "\""){
        str += cur_str;
    }
    std::cout << "key: " << str << "/t";
    return 1;
}

// Currently only floating point and arrays can be key/value pairs value 
int ValueParser(std::deque<std::string>& tokenDeque, std::vector<nlohmann::Pairs>& pairs){
    std::string cur_str = tokenDeque.front();
    tokenDeque.pop_front();

    if(cur_str == "["){
        std::cerr << "Not implemented yet\n";
        return -1;
    }
    else{
        double value = std::stod(cur_str);
        std::cout << "Value: " << value << "\n";
        return 1;
    }
}

int ObjectParser(std::deque<std::string>& tokenDeque, std::vector<nlohmann::Pairs>& pairs){
    std::string cur_str = tokenDeque.front();
    tokenDeque.pop_front();

    if(cur_str == ","){
        std::cerr << "The character ',' cannot immediatly follow the start of an object\n";
        return -1;
    }
    if (cur_str == "}"){
        return 1;
    }
    while(1){

        if(cur_str != "\""){
            std::cerr << "json object needs to start with a string\n";
            return -1;
        }
        std::string keyStr = "";
        if(!StringParser(tokenDeque, keyStr))
        {
            return -1;
        }

        cur_str = tokenDeque.front();
        tokenDeque.pop_front();
        if (cur_str != ":"){
            std::cerr << "The character ':' should always follow a key\n";
            return -1;
        }

        
        if(!ValueParser(tokenDeque, pairs)){
            return -1;
        }
        // parse value

        cur_str = tokenDeque.front();
        tokenDeque.pop_front();

        if (cur_str == "}"){
            return 1;
    }
        
        if(cur_str != ","){
            std::cerr << "Key/Value pairs have to be seperated by comma\n";
            return -1;
        }

        cur_str = tokenDeque.front();
        tokenDeque.pop_front();
    }

    // 1. parse key pairs
    // 2. 

    return 1;
}

int Parser(std::deque<std::string>& tokenDeque, PairContainer& obj){
    std::string cur_token;
    cur_token = tokenDeque.front();
    tokenDeque.pop_front();
    if (cur_token != "{"){
        std::cerr << "json object has to start with '{'\n";
        return -1;
    }
    if(!ObjectParser(tokenDeque, obj.pairs)){
        return -1;
    }
    return 1;
}


int main(){
    std::ifstream file("test.json");
    if(!file){
        std::cerr << "unable to open file";
    }

    std::deque<std::string> tokenDeque;


    char c;
    while (file.get(c)) {
        if(!Lexer(c, file, tokenDeque)){
            return -1;
        }   
    }

    PairContainer obj;
    if(!Parser(tokenDeque, obj)){
        return -1;
    }
    // for (int i = 0; i < tokenDeque.size(); i++){
    //     std::cout << tokenDeque[i];
    // }
    file.close();
}