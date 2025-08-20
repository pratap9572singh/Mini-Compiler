#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <memory>

// Defines all the different kinds of tokens our language recognizes.
enum class TokenType
{
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
struct Token
{
    TokenType type;
    std::string value;
};

// This class is responsible for turning a string of source code into a list of tokens.
class Lexer
{
public:
    Lexer(const std::string &source)
        : source_code(source), current_pos(0) {}

    Token getNextToken()
    {
        skipWhitespace();

        if (current_pos >= source_code.length())
        {
            return {TokenType::END_OF_FILE, ""};
        }

        char current_char = source_code[current_pos];

        if (current_char == '+')
        {
            current_pos++;
            return {TokenType::OPERATOR_PLUS, "+"};
        }
        if (current_char == '-')
        {
            current_pos++;
            return {TokenType::OPERATOR_MINUS, "-"};
        }
        if (current_char == '*')
        {
            current_pos++;
            return {TokenType::OPERATOR_MULTIPLY, "*"};
        }
        if (current_char == '/')
        {
            current_pos++;
            return {TokenType::OPERATOR_DIVIDE, "/"};
        }
        if (current_char == '=')
        {
            current_pos++;
            return {TokenType::OPERATOR_ASSIGN, "="};
        }
        if (current_char == ';')
        {
            current_pos++;
            return {TokenType::PUNCTUATION_SEMICOLON, ";"};
        }

        if (std::isdigit(current_char))
        {
            return readInteger();
        }

        if (std::isalpha(current_char))
        {
            return readIdentifierOrKeyword();
        }

        current_pos++;
        return {TokenType::UNKNOWN, std::string(1, current_char)};
    }

private:
    std::string source_code;
    size_t current_pos;

    void skipWhitespace()
    {
        while (current_pos < source_code.length() && std::isspace(source_code[current_pos]))
        {
            current_pos++;
        }
    }

    Token readInteger()
    {
        size_t start_pos = current_pos;
        while (current_pos < source_code.length() && std::isdigit(source_code[current_pos]))
        {
            current_pos++;
        }
        std::string value = source_code.substr(start_pos, current_pos - start_pos);
        return {TokenType::INTEGER_LITERAL, value};
    }

    Token readIdentifierOrKeyword()
    {
        size_t start_pos = current_pos;
        while (current_pos < source_code.length() && std::isalnum(source_code[current_pos]))
        {
            current_pos++;
        }
        std::string value = source_code.substr(start_pos, current_pos - start_pos);

        if (value == "int")
        {
            return {TokenType::KEYWORD_INT, value};
        }

        return {TokenType::IDENTIFIER, value};
    }
};

// Helper function to convert a TokenType to a human-readable string for printing.
std::string tokenTypeToString(TokenType type)
{
    switch (type)
    {
    case TokenType::KEYWORD_INT:
        return "KEYWORD_INT";
    case TokenType::IDENTIFIER:
        return "IDENTIFIER";
    case TokenType::INTEGER_LITERAL:
        return "INTEGER_LITERAL";
    case TokenType::OPERATOR_PLUS:
        return "OPERATOR_PLUS";
    case TokenType::OPERATOR_MINUS:
        return "OPERATOR_MINUS";
    case TokenType::OPERATOR_MULTIPLY:
        return "OPERATOR_MULTIPLY";
    case TokenType::OPERATOR_DIVIDE:
        return "OPERATOR_DIVIDE";
    case TokenType::OPERATOR_ASSIGN:
        return "OPERATOR_ASSIGN";
    case TokenType::PUNCTUATION_SEMICOLON:
        return "PUNCTUATION_SEMICOLON";
    case TokenType::END_OF_FILE:
        return "END_OF_FILE";
    case TokenType::UNKNOWN:
        return "UNKNOWN";
    default:
        return "ERROR";
    }
}

// Base struct for all Abstract Syntax Tree nodes.
struct AstNode
{
    virtual ~AstNode() = default;
};

// Represents a number literal in the code.
struct NumberNode : public AstNode
{
    Token token;
    NumberNode(Token t) : token(t) {}
};

// Represents a binary operation (e.g., 10 + 20).
struct BinaryOpNode : public AstNode
{
    std::unique_ptr<AstNode> left;
    Token op;
    std::unique_ptr<AstNode> right;
    BinaryOpNode(std::unique_ptr<AstNode> l, Token o, std::unique_ptr<AstNode> r)
        : left(std::move(l)), op(o), right(std::move(r)) {}
};

// Represents a variable declaration statement.
struct VarDeclNode : public AstNode
{
    Token type;
    Token identifier;
    std::unique_ptr<AstNode> expression;
    VarDeclNode(Token t, Token id, std::unique_ptr<AstNode> expr)
        : type(t), identifier(id), expression(std::move(expr)) {}
};

// This class takes a list of tokens and builds an Abstract Syntax Tree.
class Parser
{
public:
    Parser(const std::vector<Token> &tokens) : tokens(tokens), current_token_idx(0) {}
    std::unique_ptr<AstNode> parse() { return parseStatement(); }

private:
    std::vector<Token> tokens;
    size_t current_token_idx;

    Token &peek() { return tokens[current_token_idx]; }
    void advance()
    {
        if (current_token_idx < tokens.size())
            current_token_idx++;
    }

    std::unique_ptr<AstNode> parseStatement()
    {
        if (peek().type == TokenType::KEYWORD_INT)
            return parseVariableDeclaration();
        return nullptr;
    }

    std::unique_ptr<AstNode> parseVariableDeclaration()
    {
        Token type = peek();
        advance(); // consume int

        // ensure the next token is an identifier
        if (peek().type != TokenType::IDENTIFIER)
        {
            std::cerr << "Error: Expected an identifier after int\n";
            return nullptr;
        }
        Token identifier = peek();
        advance(); // consume the identifier

        if (peek().type != TokenType::OPERATOR_ASSIGN)
        {
            std::cerr << "Error: Expected equals sign\n";
            return nullptr;
        }
        advance(); // consume equals

        auto expression = parseExpression();
        if (!expression)
        {
            return nullptr;
        }

        if (peek().type != TokenType::PUNCTUATION_SEMICOLON)
        {
            std::cerr << "Error: Expected semicolon\n";
            return nullptr;
        }
        advance(); // consume semicolon

        return std::make_unique<VarDeclNode>(type, identifier, std::move(expression));
    }

    std::unique_ptr<AstNode> parseExpression()
    {
        auto left = parseTerm();
        while (peek().type == TokenType::OPERATOR_PLUS || peek().type == TokenType::OPERATOR_MINUS)
        {
            Token op = peek();
            advance();
            auto right = parseTerm();
            if (!right)
            {
                std::cerr << "Error: Expected a number or identifier after operator\n";
                return nullptr;
            }
            left = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
        }
        return left;
    }

    std::unique_ptr<AstNode> parseTerm()
    {
        Token token = peek();
        if (token.type == TokenType::INTEGER_LITERAL)
        {
            advance();
            return std::make_unique<NumberNode>(token);
        }
        return nullptr;
    }
};

// Helper function to print the AST for verification.
void printAst(const AstNode *node, int indent = 0)
{
    if (!node)
        return;
    std::string indentation(indent * 2, ' ');
    if (auto p = dynamic_cast<const VarDeclNode *>(node))
    {
        std::cout << indentation << "VarDecl: " << p->identifier.value << " (" << p->type.value << ")\n";
        std::cout << indentation << "  Value:\n";
        printAst(p->expression.get(), indent + 2);
    }
    else if (auto p = dynamic_cast<const BinaryOpNode *>(node))
    {
        std::cout << indentation << "BinaryOp: " << p->op.value << "\n";
        std::cout << indentation << "  Left:\n";
        printAst(p->left.get(), indent + 2);
        std::cout << indentation << "  Right:\n";
        printAst(p->right.get(), indent + 2);
    }
    else if (auto p = dynamic_cast<const NumberNode *>(node))
    {
        std::cout << indentation << "Number: " << p->token.value << "\n";
    }
}

// The entry point of our program.
int main()
{
    std::string code = "int result = 10 ;";
    std::cout << "Input Code:\n"
              << code << "\n\n";

    Lexer lexer(code);
    std::vector<Token> tokens;
    Token token;
    do
    {
        token = lexer.getNextToken();
        tokens.push_back(token);
    } while (token.type != TokenType::END_OF_FILE);

    std::cout << "Parser Output (Abstract Syntax Tree):\n";
    Parser parser(tokens);
    auto ast_root = parser.parse();
    if (!ast_root)
    {
        std::cerr << "Parsing failed.\n";
        return 1;
    }
    printAst(ast_root.get());

    return 0;
}
