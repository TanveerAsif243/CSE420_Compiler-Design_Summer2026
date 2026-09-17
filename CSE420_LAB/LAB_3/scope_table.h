#ifndef SCOPE_TABLE_H
#define SCOPE_TABLE_H

#include "symbol_info.h"

class scope_table
{
private:
    int bucket_count;
    int unique_id;
    scope_table *parent_scope;
    vector<list<symbol_info *>> table;

    int hash_function(const string &name) const
    {
        
        long long sum = 0;
        for (unsigned char c : name) sum += c;
        return (int)(sum % bucket_count);
    }

public:
    scope_table() : bucket_count(10), unique_id(0), parent_scope(nullptr), table(10) {}

    scope_table(int bucket_count, int unique_id, scope_table *parent_scope)
        : bucket_count(max(1, bucket_count)), unique_id(unique_id), parent_scope(parent_scope),
          table(max(1, bucket_count)) {}

    scope_table *get_parent_scope() { return parent_scope; }
    int get_unique_id() const { return unique_id; }

    symbol_info *lookup_in_scope(const string &name)
    {
        int idx = hash_function(name);
        for (symbol_info *s : table[idx])
            if (s->get_name() == name) return s;
        return nullptr;
    }

    symbol_info *lookup_in_scope(symbol_info *symbol)
    {
        return symbol ? lookup_in_scope(symbol->get_name()) : nullptr;
    }

    bool insert_in_scope(symbol_info *symbol)
    {
        if (!symbol || lookup_in_scope(symbol->get_name()) != nullptr) return false;
        table[hash_function(symbol->get_name())].push_back(symbol);
        return true;
    }

    bool delete_from_scope(symbol_info *symbol)
    {
        if (!symbol) return false;
        int idx = hash_function(symbol->get_name());
        for (auto it = table[idx].begin(); it != table[idx].end(); ++it)
        {
            if ((*it)->get_name() == symbol->get_name())
            {
                delete *it;
                table[idx].erase(it);
                return true;
            }
        }
        return false;
    }

    void print_scope_table(ofstream &outlog)
    {
        outlog << "ScopeTable # " << unique_id << endl;

        for (int i = 0; i < bucket_count; ++i)
        {
            if (table[i].empty()) continue;

            outlog << i << " --> " << endl;
            for (symbol_info *s : table[i])
            {
                outlog << "< " << s->get_name() << " : " << s->get_type() << " >" << endl;

                if (s->get_symbol_kind() == "function")
                {
                    outlog << "Function Definition" << endl;
                    outlog << "Return Type: " << s->get_data_type() << endl;
                    outlog << "Number of Parameters: " << s->get_parameters().size() << endl;
                    outlog << "Parameter Details: ";
                    const auto &p = s->get_parameters();
                    for (size_t j = 0; j < p.size(); ++j)
                    {
                        if (j) outlog << ", ";
                        outlog << p[j].type;
                        if (!p[j].name.empty()) outlog << " " << p[j].name;
                    }
                    outlog << endl;
                }
                else if (s->get_symbol_kind() == "array")
                {
                    outlog << "Array" << endl;
                    outlog << "Type: " << s->get_data_type() << endl;
                    outlog << "Size: " << s->get_array_size() << endl << endl;
                }
                else
                {
                    outlog << "Variable" << endl;
                    outlog << "Type: " << s->get_data_type() << endl << endl;
                }
            }
        }
        outlog << endl;
    }

    ~scope_table()
    {
        for (auto &bucket : table)
            for (symbol_info *s : bucket)
                delete s;
    }
};

#endif
