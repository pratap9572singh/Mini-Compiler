#include <iostream>
#include <string>
#include <vector>
#include <cctype>

// Defines all the different kinds of tokens our language recognizes.
enum class TokenType {
    KEYWORD_INT,
    IDENTIFIER,
    INTEGER_LITERAL,
    OPERATOR_PLUS,
    OPERATOR_MINUS,
    OPERATOR_MULTIPLY,
    OPERATOR_DIVIDE,
    OPERATOR_ASSIGN,
    PUNCTUATION_SEMICOLON,
    END_OF_FILE,
    UNKNOWN
};

// Represents a single token with its type and the actual text (value).
struct Token {
    TokenType type;
    std::string value;
};

// This class is responsible for turning a string of source code into a list of tokens.
class Lexer {
public:
    Lexer(const std::string& source)
        : source_code(source), current_pos(0) {}

    Token getNextToken() {
        skipWhitespace();

        if (current_pos >= source_code.length()) {
            return {TokenType::END_OF_FILE, ""};
        }

        char current_char = source_code[current_pos];

        if (current_char == '+') {
            current_pos++;
            return {TokenType::OPERATOR_PLUS, "+"};
        }
        if (current_char == '-') {
            current_pos++;
            return {TokenType::OPERATOR_MINUS, "-"};
        }
        if (current_char == '*') {
            current_pos++;
            return {TokenType::OPERATOR_MULTIPLY, "*"};
        }
        if (current_char == '/') {
            current_pos++;
            return {TokenType::OPERATOR_DIVIDE, "/"};
        }
        if (current_char == '=') {
            current_pos++;
            return {TokenType::OPERATOR_ASSIGN, "="};
        }
        if (current_char == ';') {
            current_pos++;
            return {TokenType::PUNCTUATION_SEMICOLON, ";"};
        }

        if (std::isdigit(current_char)) {
            return readInteger();
        }

        if (std::isalpha(current_char)) {
            return readIdentifierOrKeyword();
        }

        current_pos++;
        return {TokenType::UNKNOWN, std::string(1, current_char)};
    }

private:
    std::string source_code;
    size_t current_pos;

    void skipWhitespace() {
        while (current_pos < source_code.length() && std::isspace(source_code[current_pos])) {
            current_pos++;
        }
    }

    Token readInteger() {
        size_t start_pos = current_pos;
        while (current_pos < source_code.length() && std::isdigit(source_code[current_pos])) {
            current_pos++;
        }
        std::string value = source_code.substr(start_pos, current_pos - start_pos);
        return {TokenType::INTEGER_LITERAL, value};
    }

    Token readIdentifierOrKeyword() {
        size_t start_pos = current_pos;
        while (current_pos < source_code.length() && std::isalnum(source_code[current_pos])) {
            current_pos++;
        }
        std::string value = source_code.substr(start_pos, current_pos - start_pos);

        if (value == "int") {
            return {TokenType::KEYWORD_INT, value};
        }

        return {TokenType::IDENTIFIER, value};
    }
};

// Helper function to convert a TokenType to a human-readable string for printing.
std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::KEYWORD_INT:         return "KEYWORD_INT";
        case TokenType::IDENTIFIER:          return "IDENTIFIER";
        case TokenType::INTEGER_LITERAL:     return "INTEGER_LITERAL";
        case TokenType::OPERATOR_PLUS:       return "OPERATOR_PLUS";
        case TokenType::OPERATOR_MINUS:      return "OPERATOR_MINUS";
        case TokenType::OPERATOR_MULTIPLY:   return "OPERATOR_MULTIPLY";
        case TokenType::OPERATOR_DIVIDE:     return "OPERATOR_DIVIDE";
        case TokenType::OPERATOR_ASSIGN:     return "OPERATOR_ASSIGN";
        case TokenType::PUNCTUATION_SEMICOLON: return "PUNCTUATION_SEMICOLON";
        case TokenType::END_OF_FILE:         return "END_OF_FILE";
        case TokenType::UNKNOWN:             return "UNKNOWN";
        default:                             return "ERROR";
    }
}

// The entry point of our program.
int main() { 
    std::string code = "int result = 10 + 20;";

    std::cout << "Tokenizing the following code:\n\"" << code << "\"\n\n";

    Lexer lexer(code);

    Token token;
    do {
        token = lexer.getNextToken();
        std::cout << "Type: " << tokenTypeToString(token.type)
                  << ", Value: '" << token.value << "'\n";
    } while (token.type != TokenType::END_OF_FILE);

    return 0;
}
