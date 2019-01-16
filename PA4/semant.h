#ifndef SEMANT_H_
#define SEMANT_H_

#include <assert.h>
#include <iostream>
#include "cool-tree.h"
#include "stringtab.h"
#include "symtab.h"
#include "list.h"

#define TRUE 1
#define FALSE 0

class ClassTable;
typedef ClassTable *ClassTableP;

// This is a structure that may be used to contain the semantic
// information such as the inheritance graph.  You may use it or not as
// you like: it is only here to provide a container for the supplied
// methods.

class ClassTable {
private:
  int semant_errors;
  void install_basic_classes();
  ostream& error_stream;
  // Codes added
  Classes all_classes;
  int class_num;
  SymbolTable<Symbol, int> *class_names;
  SymbolTable<int, int> *inheritance;
  SymbolTable<Symbol, Class__class> *classes_scope;
  SymbolTable<Symbol, SymbolTable<Symbol, Entry> > *attr_declarations;
  SymbolTable<Symbol, SymbolTable<Symbol, Feature_class> > *method_declarations;
  void build_inheritance_graph();
  bool isCyclicUtil(int v, bool visited[], bool *recStack);
  bool isCyclic();

public:
  ClassTable(Classes);
  int errors() { return semant_errors; }
  ostream& semant_error();
  ostream& semant_error(Class_ c);
  ostream& semant_error(Symbol filename, tree_node *t);
  SymbolTable<Symbol, Class__class> *get_all_classes();
  void naming_scoping();
  void naming_scoping_helper(Class_ curr_class, SymbolTable<Symbol, Entry> *curr_attr_declarations, SymbolTable<Symbol, Feature_class> *curr_method_declarations, bool error_flag);
  void naming_scoping_acc(Class_ curr_class, SymbolTable<Symbol, Entry> *curr_attr_declarations, SymbolTable<Symbol, Feature_class> *curr_method_declarations);
  void naming_scoping(Class_ curr_class);
  void naming_scoping(Feature curr_feature, Symbol filename, SymbolTable<Symbol, Entry> *curr_attr_declarations, SymbolTable<Symbol, Feature_class> *curr_method_declarations, bool error_flag);
  void type_checking();
  bool is_subclass(Symbol name1, Symbol name2);
  bool no_type_error(Symbol filename, Symbol curr_type, tree_node *t);
  Symbol lub(Symbol T1, Symbol T2);
  bool no_method_error(Symbol class_name, Symbol method_name, Symbol filename, Expression curr_expression);
  Feature get_method_entity(Symbol class_name, Symbol method_name);
  void halt();
};


#endif
