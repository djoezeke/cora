#ifndef TOY_AST_NODE_H
#define TOY_AST_NODE_H

#include <iostream>
#include <vector>

#include "../lexer/tokens.hpp"

#define LLVM_VALUE Value

#define BEGIN_NAMESPACE \
    namespace toy       \
    {

#define END_NAMESPACE } // toy

#define GET_TYPE(type) \
    NodeType getType() override { return NodeType::type; }

#define STR_TYPE(type) \
    std::string strType() override { return #type; }

#define ACCEPT() \
    void Accept(Visitor &visitor) override { visitor.VisitString(this); }

#define CODEGEN() LLVM_VALUE *codeGen(CodeGenContext &context) override;

struct YYLTYPE
{
    int first_line{0};
    int first_column{0};
    int last_line{0};
    int last_column{0};
    std::string file_name;
};

BEGIN_NAMESPACE

class Visitor;
class CodeGenContext;
class Value;

/*! Type of the AST node */
enum class NodeType
{

    Expression,
    Statement,

    Identifier,
    Document,
    Integer,
    Double,
    Float,
    String,
    Byte,
    Bool,
    None,

    // Binary Operation
    BinaryOperation,

    Add,
    Sub,
    Mul,
    Div,

    BitAnd,
    BitOr,
    BitXor,

    LShift,
    RShift,

    Mod,
    Pow,
    FloorDiv,

    // Unary Operations
    UnaryOperation,

    UAdd,
    USub,

    Invert,
    Not,

    // Comparission s
    Comp,

    EQ,
    NEQ,

    GT,
    GTE,

    LT,
    LTE,

    In,
    NotIn,

    Is,
    IsNot,

    // Modification
    Modification,

    Const,

    Long,
    Short,

    Signed,
    Unsigned,

    Static,
    Extern,

};

class Node
{
public:
    virtual ~Node() = default;

    // /*! Returns the type of the node. */
    virtual NodeType getType() = 0;

    // /*! Returns the textual representation. */
    virtual std::string strType() { return "Node"; }

    /*! Accept a visitor. */
    virtual void Accept(Visitor &visitor) = 0;

    /*! Code generation for this node
     * \param[in] context  The context of the code gen run.
     * \return Generated code as LLVM value.
     */
    virtual LLVM_VALUE *codeGen(CodeGenContext &context) = 0;

    /*! Prints the found parsing error.
     * \param[in] location file, line, col information.
     * \param[in] msg      The message to print.
     */
    static void printError(YYLTYPE location, std::string msg)
    {
        std::cerr
            << location.file_name
            << ": line "
            << location.first_line << " column "
            << location.first_column << "-"
            << location.last_column << ":"
            << msg << std::endl;
    }

    /*! Prints an error message where no source location is available.
     * \param[in] msg      The message to print.
     */
    static void printError(std::string msg)
    {
        std::cerr << msg << std::endl;
    }
};

END_NAMESPACE

#endif // TOY_AST_NODE_H
