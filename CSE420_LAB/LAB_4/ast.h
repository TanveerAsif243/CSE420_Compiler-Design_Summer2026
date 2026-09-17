#ifndef AST_H
#define AST_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <utility>

using namespace std;

inline string ast_indent(int indent) {
    return string(indent, ' ');
}

inline string new_temp(int& temp_count) {
    return "t" + to_string(temp_count++);
}

inline string new_label(int& label_count) {
    return "L" + to_string(label_count++);
}

// Returns true only for compiler-generated temporaries such as t0, t1, ...
inline bool is_temporary_name(const string& value) {
    if (value.size() < 2 || value[0] != 't') return false;
    for (size_t i = 1; i < value.size(); ++i) {
        if (value[i] < '0' || value[i] > '9') return false;
    }
    return true;
}

// Load a raw operand (identifier/constant) into a temporary.
// If the operand is already a temporary, simply reuse it.
inline string ensure_temp(ofstream& outcode, const string& value, int& temp_count) {
    if (is_temporary_name(value)) return value;
    string temp = new_temp(temp_count);
    outcode << temp << " = " << value << '\n';
    return temp;
}

class ASTNode {
public:
    virtual ~ASTNode() {}
    virtual string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                                 int& temp_count, int& label_count) const = 0;
    virtual void print_ast(ostream& out, int indent = 0) const = 0;
};

// Expression node types
class ExprNode : public ASTNode {
protected:
    string node_type;
public:
    explicit ExprNode(string type) : node_type(type) {}
    virtual string get_type() const { return node_type; }
};

// Variable node (for ID references and array access)
class VarNode : public ExprNode {
private:
    string name;
    ExprNode* index;

public:
    VarNode(string name, string type, ExprNode* idx = nullptr)
        : ExprNode(type), name(name), index(idx) {}

    ~VarNode() override { delete index; }

    bool has_index() const { return index != nullptr; }
    string get_name() const { return name; }

    string generate_index_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                               int& temp_count, int& label_count) const {
        if (!index) return "";
        return index->generate_code(outcode, symbol_to_temp, temp_count, label_count);
    }

    // Generates an l-value without loading the array element into a temporary.
    string generate_lvalue_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                                int& temp_count, int& label_count) const {
        if (!index) return name;
        string idx = generate_index_code(outcode, symbol_to_temp, temp_count, label_count);
        return name + "[" + idx + "]";
    }

    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                         int& temp_count, int& label_count) const override {
        if (!index) {
            // Function parameters are loaded into temporaries at function entry.
            auto it = symbol_to_temp.find(name);
            if (it != symbol_to_temp.end()) return it->second;
            return name;
        }

        string lvalue = generate_lvalue_code(outcode, symbol_to_temp, temp_count, label_count);
        string temp = new_temp(temp_count);
        outcode << temp << " = " << lvalue << '\n';
        return temp;
    }

    void print_ast(ostream& out, int indent = 0) const override {
        out << ast_indent(indent) << "VarNode: " << name << " <" << node_type << ">";
        if (index) {
            out << " [indexed]" << '\n';
            out << ast_indent(indent + 2) << "Index:" << '\n';
            index->print_ast(out, indent + 4);
        } else {
            out << '\n';
        }
    }
};

// Constant node
class ConstNode : public ExprNode {
private:
    string value;

public:
    ConstNode(string val, string type) : ExprNode(type), value(val) {}

    string generate_code(ofstream&, map<string, string>&,
                         int&, int&) const override {
        return value;
    }

    void print_ast(ostream& out, int indent = 0) const override {
        out << ast_indent(indent) << "ConstNode: " << value << " <" << node_type << ">" << '\n';
    }
};

// Binary operation node
class BinaryOpNode : public ExprNode {
private:
    string op;
    ExprNode* left;
    ExprNode* right;

public:
    BinaryOpNode(string op, ExprNode* left, ExprNode* right, string result_type)
        : ExprNode(result_type), op(op), left(left), right(right) {}

    ~BinaryOpNode() override {
        delete left;
        delete right;
    }

    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                         int& temp_count, int& label_count) const override {
        string left_value = left ? left->generate_code(outcode, symbol_to_temp, temp_count, label_count) : "0";
        string right_value = right ? right->generate_code(outcode, symbol_to_temp, temp_count, label_count) : "0";

        // Three-address form used by the expected output:
        // raw operands are first copied into temporaries.
        left_value = ensure_temp(outcode, left_value, temp_count);
        right_value = ensure_temp(outcode, right_value, temp_count);

        string temp = new_temp(temp_count);
        outcode << temp << " = " << left_value << " " << op << " " << right_value << '\n';
        return temp;
    }

    void print_ast(ostream& out, int indent = 0) const override {
        out << ast_indent(indent) << "BinaryOpNode: " << op << " <" << node_type << ">" << '\n';
        if (left) left->print_ast(out, indent + 2);
        if (right) right->print_ast(out, indent + 2);
    }
};

// Unary operation node
class UnaryOpNode : public ExprNode {
private:
    string op;
    ExprNode* expr;

public:
    UnaryOpNode(string op, ExprNode* expr, string result_type)
        : ExprNode(result_type), op(op), expr(expr) {}

    ~UnaryOpNode() override { delete expr; }

    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                         int& temp_count, int& label_count) const override {
        // Post-increment/decrement are represented here to avoid duplicating ownership
        // of the same VarNode in an AssignNode/BinaryOpNode tree.
        if (op == "post++" || op == "post--") {
            const VarNode* var = dynamic_cast<const VarNode*>(expr);
            if (var) {
                string lvalue = var->generate_lvalue_code(outcode, symbol_to_temp, temp_count, label_count);
                string old_value = new_temp(temp_count);
                outcode << old_value << " = " << lvalue << '\n';
                outcode << lvalue << " = " << old_value << (op == "post++" ? " + 1" : " - 1") << '\n';
                return old_value;
            }
        }

        string value = expr ? expr->generate_code(outcode, symbol_to_temp, temp_count, label_count) : "0";
        value = ensure_temp(outcode, value, temp_count);
        string temp = new_temp(temp_count);
        outcode << temp << " = " << op << value << '\n';
        return temp;
    }

    void print_ast(ostream& out, int indent = 0) const override {
        out << ast_indent(indent) << "UnaryOpNode: " << op << " <" << node_type << ">" << '\n';
        if (expr) expr->print_ast(out, indent + 2);
    }
};

// Assignment node
class AssignNode : public ExprNode {
private:
    VarNode* lhs;
    ExprNode* rhs;

public:
    AssignNode(VarNode* lhs, ExprNode* rhs, string result_type)
        : ExprNode(result_type), lhs(lhs), rhs(rhs) {}

    ~AssignNode() override {
        delete lhs;
        delete rhs;
    }

    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                         int& temp_count, int& label_count) const override {
        if (!lhs) return "";

        string lvalue = lhs->generate_lvalue_code(outcode, symbol_to_temp, temp_count, label_count);
        string value = rhs ? rhs->generate_code(outcode, symbol_to_temp, temp_count, label_count) : "0";

        // Constants and plain identifiers are first materialized in a temporary.
        // Expression/function-call results already arrive as temporaries.
        value = ensure_temp(outcode, value, temp_count);
        outcode << lvalue << " = " << value << '\n';

        // If a formal parameter is assigned a new value, its function-entry
        // alias is no longer valid for later reads.
        if (!lhs->has_index()) {
            symbol_to_temp.erase(lhs->get_name());
        }

        return value;
    }

    void print_ast(ostream& out, int indent = 0) const override {
        out << ast_indent(indent) << "AssignNode: = <" << node_type << ">" << '\n';
        out << ast_indent(indent + 2) << "LHS:" << '\n';
        if (lhs) lhs->print_ast(out, indent + 4);
        out << ast_indent(indent + 2) << "RHS:" << '\n';
        if (rhs) rhs->print_ast(out, indent + 4);
    }
};

// Statement node types
class StmtNode : public ASTNode {
public:
    virtual ~StmtNode() {}
};

// Expression statement node
class ExprStmtNode : public StmtNode {
private:
    ExprNode* expr;

public:
    explicit ExprStmtNode(ExprNode* e) : expr(e) {}
    ~ExprStmtNode() override { delete expr; }

    // Used by the parser when the expression inside a for-clause must be
    // transferred into a ForNode while preserving the skeleton's ExprNode fields.
    ExprNode* release_expr() {
        ExprNode* result = expr;
        expr = nullptr;
        return result;
    }

    const ExprNode* get_expr() const { return expr; }

    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                         int& temp_count, int& label_count) const override {
        if (expr) expr->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        return "";
    }

    void print_ast(ostream& out, int indent = 0) const override {
        out << ast_indent(indent) << "ExprStmtNode" << '\n';
        if (expr) expr->print_ast(out, indent + 2);
        else out << ast_indent(indent + 2) << "<empty>" << '\n';
    }
};

// Block (compound statement) node
class BlockNode : public StmtNode {
private:
    vector<StmtNode*> statements;

public:
    ~BlockNode() override {
        for (auto stmt : statements) delete stmt;
    }

    void add_statement(StmtNode* stmt) {
        if (stmt) statements.push_back(stmt);
    }

    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                         int& temp_count, int& label_count) const override {
        for (auto stmt : statements) {
            if (stmt) stmt->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        }
        return "";
    }

    void print_ast(ostream& out, int indent = 0) const override {
        out << ast_indent(indent) << "BlockNode" << '\n';
        for (auto stmt : statements) {
            if (stmt) stmt->print_ast(out, indent + 2);
        }
    }
};

// If statement node
class IfNode : public StmtNode {
private:
    ExprNode* condition;
    StmtNode* then_block;
    StmtNode* else_block;

public:
    IfNode(ExprNode* cond, StmtNode* then_stmt, StmtNode* else_stmt = nullptr)
        : condition(cond), then_block(then_stmt), else_block(else_stmt) {}

    ~IfNode() override {
        delete condition;
        delete then_block;
        delete else_block;
    }

    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                         int& temp_count, int& label_count) const override {
        string cond_value = condition ? condition->generate_code(outcode, symbol_to_temp, temp_count, label_count) : "1";
        cond_value = ensure_temp(outcode, cond_value, temp_count);
        string then_label = new_label(label_count);
        string end_label = new_label(label_count);

        if (else_block) {
            string else_label = new_label(label_count);
            outcode << "if " << cond_value << " goto " << then_label << '\n';
            outcode << "goto " << else_label << '\n';
            outcode << then_label << ":" << '\n';
            then_block->generate_code(outcode, symbol_to_temp, temp_count, label_count);
            outcode << "goto " << end_label << '\n';
            outcode << else_label << ":" << '\n';
            else_block->generate_code(outcode, symbol_to_temp, temp_count, label_count);
            outcode << end_label << ":" << '\n';
        } else {
            outcode << "if " << cond_value << " goto " << then_label << '\n';
            outcode << "goto " << end_label << '\n';
            outcode << then_label << ":" << '\n';
            then_block->generate_code(outcode, symbol_to_temp, temp_count, label_count);
            outcode << end_label << ":" << '\n';
        }
        return "";
    }

    void print_ast(ostream& out, int indent = 0) const override {
        out << ast_indent(indent) << "IfNode" << '\n';
        out << ast_indent(indent + 2) << "Condition:" << '\n';
        if (condition) condition->print_ast(out, indent + 4);
        out << ast_indent(indent + 2) << "Then:" << '\n';
        if (then_block) then_block->print_ast(out, indent + 4);
        if (else_block) {
            out << ast_indent(indent + 2) << "Else:" << '\n';
            else_block->print_ast(out, indent + 4);
        }
    }
};

// While statement node
class WhileNode : public StmtNode {
private:
    ExprNode* condition;
    StmtNode* body;

public:
    WhileNode(ExprNode* cond, StmtNode* body_stmt)
        : condition(cond), body(body_stmt) {}

    ~WhileNode() override {
        delete condition;
        delete body;
    }

    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                         int& temp_count, int& label_count) const override {
        string cond_label = new_label(label_count);
        string body_label = new_label(label_count);
        string end_label = new_label(label_count);

        outcode << cond_label << ":" << '\n';
        string cond_value = condition ? condition->generate_code(outcode, symbol_to_temp, temp_count, label_count) : "1";
        cond_value = ensure_temp(outcode, cond_value, temp_count);
        outcode << "if " << cond_value << " goto " << body_label << '\n';
        outcode << "goto " << end_label << '\n';
        outcode << body_label << ":" << '\n';
        if (body) body->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        outcode << "goto " << cond_label << '\n';
        outcode << end_label << ":" << '\n';
        return "";
    }

    void print_ast(ostream& out, int indent = 0) const override {
        out << ast_indent(indent) << "WhileNode" << '\n';
        out << ast_indent(indent + 2) << "Condition:" << '\n';
        if (condition) condition->print_ast(out, indent + 4);
        out << ast_indent(indent + 2) << "Body:" << '\n';
        if (body) body->print_ast(out, indent + 4);
    }
};

// For statement node
class ForNode : public StmtNode {
private:
    ExprNode* init;
    ExprNode* condition;
    ExprNode* update;
    StmtNode* body;

public:
    ForNode(ExprNode* init_expr, ExprNode* cond_expr, ExprNode* update_expr, StmtNode* body_stmt)
        : init(init_expr), condition(cond_expr), update(update_expr), body(body_stmt) {}

    ~ForNode() override {
        delete init;
        delete condition;
        delete update;
        delete body;
    }

    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                         int& temp_count, int& label_count) const override {
        if (init) init->generate_code(outcode, symbol_to_temp, temp_count, label_count);

        string cond_label = new_label(label_count);
        string body_label = new_label(label_count);
        string update_label = new_label(label_count);
        string end_label = new_label(label_count);

        outcode << cond_label << ":" << '\n';
        if (condition) {
            string cond_value = condition->generate_code(outcode, symbol_to_temp, temp_count, label_count);
            cond_value = ensure_temp(outcode, cond_value, temp_count);
            outcode << "if " << cond_value << " goto " << body_label << '\n';
            outcode << "goto " << end_label << '\n';
        } else {
            outcode << "goto " << body_label << '\n';
        }

        outcode << body_label << ":" << '\n';
        if (body) body->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        outcode << update_label << ":" << '\n';
        if (update) update->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        outcode << "goto " << cond_label << '\n';
        outcode << end_label << ":" << '\n';
        return "";
    }

    void print_ast(ostream& out, int indent = 0) const override {
        out << ast_indent(indent) << "ForNode" << '\n';
        out << ast_indent(indent + 2) << "Init:" << '\n';
        if (init) init->print_ast(out, indent + 4);
        else out << ast_indent(indent + 4) << "<empty>" << '\n';
        out << ast_indent(indent + 2) << "Condition:" << '\n';
        if (condition) condition->print_ast(out, indent + 4);
        else out << ast_indent(indent + 4) << "<empty/true>" << '\n';
        out << ast_indent(indent + 2) << "Update:" << '\n';
        if (update) update->print_ast(out, indent + 4);
        else out << ast_indent(indent + 4) << "<empty>" << '\n';
        out << ast_indent(indent + 2) << "Body:" << '\n';
        if (body) body->print_ast(out, indent + 4);
    }
};

// Return statement node
class ReturnNode : public StmtNode {
private:
    ExprNode* expr;

public:
    explicit ReturnNode(ExprNode* e) : expr(e) {}
    ~ReturnNode() override { delete expr; }

    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                         int& temp_count, int& label_count) const override {
        if (expr) {
            string value = expr->generate_code(outcode, symbol_to_temp, temp_count, label_count);
            value = ensure_temp(outcode, value, temp_count);
            outcode << "return " << value << '\n';
        } else {
            outcode << "return" << '\n';
        }
        return "";
    }

    void print_ast(ostream& out, int indent = 0) const override {
        out << ast_indent(indent) << "ReturnNode" << '\n';
        if (expr) expr->print_ast(out, indent + 2);
    }
};

// Special statement used by the existing grammar's printf(ID); rule.
class PrintNode : public StmtNode {
private:
    ExprNode* expr;

public:
    explicit PrintNode(ExprNode* e) : expr(e) {}
    ~PrintNode() override { delete expr; }

    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                         int& temp_count, int& label_count) const override {
        string value = expr ? expr->generate_code(outcode, symbol_to_temp, temp_count, label_count) : "0";
        value = ensure_temp(outcode, value, temp_count);
        outcode << "param " << value << '\n';
        outcode << "call printf, 1" << '\n';
        return "";
    }

    void print_ast(ostream& out, int indent = 0) const override {
        out << ast_indent(indent) << "PrintNode" << '\n';
        if (expr) expr->print_ast(out, indent + 2);
    }
};

// Declaration node
class DeclNode : public StmtNode {
private:
    string type;
    vector<pair<string, int>> vars;

public:
    explicit DeclNode(string t) : type(t) {}

    void add_var(string name, int array_size = 0) {
        vars.push_back(make_pair(name, array_size));
    }

    string generate_code(ofstream& outcode, map<string, string>&,
                         int&, int&) const override {
        for (const auto& item : vars) {
            outcode << "// Declaration: " << type << " " << item.first;
            if (item.second > 0) outcode << "[" << item.second << "]";
            outcode << '\n';
        }
        return "";
    }

    string get_type() const { return type; }
    const vector<pair<string, int>>& get_vars() const { return vars; }

    void print_ast(ostream& out, int indent = 0) const override {
        out << ast_indent(indent) << "DeclNode: " << type << '\n';
        for (const auto& item : vars) {
            out << ast_indent(indent + 2) << item.first;
            if (item.second > 0) out << "[" << item.second << "]";
            out << '\n';
        }
    }
};

// Function declaration/definition node
class FuncDeclNode : public ASTNode {
private:
    string return_type;
    string name;
    vector<pair<string, string>> params;
    BlockNode* body;

public:
    FuncDeclNode(string ret_type, string n) : return_type(ret_type), name(n), body(nullptr) {}
    ~FuncDeclNode() override { delete body; }

    void add_param(string type, string name) {
        params.push_back(make_pair(type, name));
    }

    void set_body(BlockNode* b) { body = b; }

    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                         int& temp_count, int& label_count) const override {
        // Parameter aliases are local to each function.
        symbol_to_temp.clear();

        outcode << "// Function: " << return_type << " " << name << "(";
        for (size_t i = 0; i < params.size(); ++i) {
            if (i > 0) outcode << ", ";
            outcode << params[i].first << " " << params[i].second;
        }
        outcode << ")" << '\n';

        // Load formal parameters into temporaries at function entry.
        for (const auto& param : params) {
            string temp = new_temp(temp_count);
            outcode << temp << " = " << param.second << '\n';
            symbol_to_temp[param.second] = temp;
        }

        if (body) body->generate_code(outcode, symbol_to_temp, temp_count, label_count);

        symbol_to_temp.clear();
        return "";
    }

    void print_ast(ostream& out, int indent = 0) const override {
        out << ast_indent(indent) << "FuncDeclNode: " << return_type << " " << name << '\n';
        if (!params.empty()) {
            out << ast_indent(indent + 2) << "Parameters:" << '\n';
            for (const auto& param : params) {
                out << ast_indent(indent + 4) << param.first << " " << param.second << '\n';
            }
        }
        out << ast_indent(indent + 2) << "Body:" << '\n';
        if (body) body->print_ast(out, indent + 4);
    }
};

// Helper class for function arguments
class ArgumentsNode : public ASTNode {
private:
    vector<ExprNode*> args;

public:
    ~ArgumentsNode() override {
        // Argument ownership is transferred to FuncCallNode by the parser.
    }

    void add_argument(ExprNode* arg) {
        if (arg) args.push_back(arg);
    }

    ExprNode* get_argument(int index) const {
        if (index >= 0 && static_cast<size_t>(index) < args.size()) return args[index];
        return nullptr;
    }

    size_t size() const { return args.size(); }
    const vector<ExprNode*>& get_arguments() const { return args; }

    string generate_code(ofstream&, map<string, string>&,
                         int&, int&) const override {
        return "";
    }

    void print_ast(ostream& out, int indent = 0) const override {
        out << ast_indent(indent) << "ArgumentsNode" << '\n';
        for (auto arg : args) if (arg) arg->print_ast(out, indent + 2);
    }
};

// Function call node
class FuncCallNode : public ExprNode {
private:
    string func_name;
    vector<ExprNode*> arguments;

public:
    FuncCallNode(string name, string result_type)
        : ExprNode(result_type), func_name(name) {}

    ~FuncCallNode() override {
        for (auto arg : arguments) delete arg;
    }

    void add_argument(ExprNode* arg) {
        if (arg) arguments.push_back(arg);
    }

    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                         int& temp_count, int& label_count) const override {
        for (auto arg : arguments) {
            string value = arg ? arg->generate_code(outcode, symbol_to_temp, temp_count, label_count) : "0";

            // Plain arguments are copied into temporaries before PARAM.
            // Emit PARAM immediately so the output is:
            // tN = arg
            // param tN
            value = ensure_temp(outcode, value, temp_count);
            outcode << "param " << value << '\n';
        }

        if (node_type == "void") {
            outcode << "call " << func_name << ", " << arguments.size() << '\n';
            return "";
        }

        string temp = new_temp(temp_count);
        outcode << temp << " = call " << func_name << ", " << arguments.size() << '\n';
        return temp;
    }

    void print_ast(ostream& out, int indent = 0) const override {
        out << ast_indent(indent) << "FuncCallNode: " << func_name << " <" << node_type << ">" << '\n';
        for (auto arg : arguments) if (arg) arg->print_ast(out, indent + 2);
    }
};

// Program node (root of AST)
class ProgramNode : public ASTNode {
private:
    vector<ASTNode*> units;

public:
    ~ProgramNode() override {
        for (auto unit : units) delete unit;
    }

    void add_unit(ASTNode* unit) {
        if (unit) units.push_back(unit);
    }

    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                         int& temp_count, int& label_count) const override {
        for (size_t i = 0; i < units.size(); ++i) {
            if (units[i]) units[i]->generate_code(outcode, symbol_to_temp, temp_count, label_count);
            if (i + 1 < units.size()) outcode << '\n';
        }
        return "";
    }

    void print_ast(ostream& out, int indent = 0) const override {
        out << ast_indent(indent) << "ProgramNode" << '\n';
        for (auto unit : units) if (unit) unit->print_ast(out, indent + 2);
    }
};

#endif // AST_H
