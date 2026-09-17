#ifndef SYMBOL_INFO_H
#define SYMBOL_INFO_H

#include <bits/stdc++.h>
using namespace std;

struct parameter_info
{
    string type;
    string name;

    parameter_info(string type = "", string name = "") : type(type), name(name) {}
};

struct declaration_info
{
    string name;
    bool is_array;
    int array_size;

    declaration_info(string name = "", bool is_array = false, int array_size = -1)
        : name(name), is_array(is_array), array_size(array_size) {}
};

class symbol_info
{
private:
    string name;
    string type;                  // token / grammar-node type
    string symbol_kind;           // variable / array / function
    string data_type;             // int / float / char / void / error
    int array_size;
    vector<parameter_info> parameters;
    vector<declaration_info> declarations;
    vector<string> argument_types;
    bool constant_known;
    double constant_value;

public:
    symbol_info(string name = "", string type = "")
        : name(name), type(type), symbol_kind(""), data_type(""), array_size(-1),
          constant_known(false), constant_value(0.0) {}

    string get_name() const { return name; }
    string get_type() const { return type; }
    void set_name(const string &name) { this->name = name; }
    void set_type(const string &type) { this->type = type; }

    // Aliases kept because the supplied Yacc starter uses getname()/gettype().
    string getname() const { return name; }
    string gettype() const { return type; }
    void setname(const string &name) { this->name = name; }
    void settype(const string &type) { this->type = type; }

    string get_symbol_kind() const { return symbol_kind; }
    void set_symbol_kind(const string &kind) { symbol_kind = kind; }

    string get_data_type() const { return data_type; }
    void set_data_type(const string &dt) { data_type = dt; }

    int get_array_size() const { return array_size; }
    void set_array_size(int n) { array_size = n; }

    const vector<parameter_info> &get_parameters() const { return parameters; }
    void set_parameters(const vector<parameter_info> &p) { parameters = p; }
    void add_parameter(const parameter_info &p) { parameters.push_back(p); }

    const vector<declaration_info> &get_declarations() const { return declarations; }
    void set_declarations(const vector<declaration_info> &d) { declarations = d; }
    void add_declaration(const declaration_info &d) { declarations.push_back(d); }

    const vector<string> &get_argument_types() const { return argument_types; }
    void set_argument_types(const vector<string> &a) { argument_types = a; }
    void add_argument_type(const string &a) { argument_types.push_back(a); }

    bool is_constant_known() const { return constant_known; }
    double get_constant_value() const { return constant_value; }
    void set_constant(double v)
    {
        constant_known = true;
        constant_value = v;
    }
    void clear_constant()
    {
        constant_known = false;
        constant_value = 0.0;
    }
};

#endif
