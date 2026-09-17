#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "scope_table.h"

class symbol_table
{
private:
    scope_table *current_scope;
    int bucket_count;
    int current_scope_id;

public:
    explicit symbol_table(int bucket_count)
        : current_scope(nullptr), bucket_count(max(1, bucket_count)), current_scope_id(0) {}

    ~symbol_table()
    {
        while (current_scope != nullptr)
        {
            scope_table *parent = current_scope->get_parent_scope();
            delete current_scope;
            current_scope = parent;
        }
    }

    void enter_scope(ofstream &outlog)
    {
        current_scope = new scope_table(bucket_count, ++current_scope_id, current_scope);
        outlog << "New ScopeTable with ID " << current_scope_id << " created" << endl << endl;
    }

    void enter_scope()
    {
        current_scope = new scope_table(bucket_count, ++current_scope_id, current_scope);
    }

    void exit_scope(ofstream &outlog)
    {
        if (!current_scope) return;
        int id = current_scope->get_unique_id();
        scope_table *parent = current_scope->get_parent_scope();
        delete current_scope;
        current_scope = parent;
        outlog << "Scopetable with ID " << id << " removed" << endl << endl;
    }

    void exit_scope()
    {
        if (!current_scope) return;
        scope_table *parent = current_scope->get_parent_scope();
        delete current_scope;
        current_scope = parent;
    }

    bool insert(symbol_info *symbol)
    {
        return current_scope && current_scope->insert_in_scope(symbol);
    }

    symbol_info *lookup_current(const string &name)
    {
        return current_scope ? current_scope->lookup_in_scope(name) : nullptr;
    }

    symbol_info *lookup(const string &name)
    {
        for (scope_table *s = current_scope; s != nullptr; s = s->get_parent_scope())
        {
            symbol_info *found = s->lookup_in_scope(name);
            if (found) return found;
        }
        return nullptr;
    }

    symbol_info *lookup(symbol_info *symbol)
    {
        return symbol ? lookup(symbol->get_name()) : nullptr;
    }

    void print_current_scope(ofstream &outlog)
    {
        if (current_scope) current_scope->print_scope_table(outlog);
    }

    void print_current_scope() {}

    void print_all_scopes(ofstream &outlog)
    {
        outlog << "################################" << endl << endl;
        for (scope_table *s = current_scope; s != nullptr; s = s->get_parent_scope())
            s->print_scope_table(outlog);
        outlog << "################################" << endl << endl;
    }
};

#endif
