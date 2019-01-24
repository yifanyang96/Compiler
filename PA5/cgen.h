#include <assert.h>
#include <stdio.h>
#include "emit.h"
#include "cool-tree.h"
#include "symtab.h"

enum Basicness     {Basic, NotBasic};
#define TRUE 1
#define FALSE 0

class CgenClassTable;
typedef CgenClassTable *CgenClassTableP;

class CgenNode;
typedef CgenNode *CgenNodeP;

class CgenClassTable : public SymbolTable<Symbol,CgenNode> {
private:
   List<CgenNode> *nds;
   ostream& str;
   int stringclasstag;
   int intclasstag;
   int boolclasstag;
   int class_num;
   int label_index;


// The following methods emit code for
// constants and global declarations.

   void code_global_data();
   void code_global_text();
   void code_bools(int);
   void code_select_gc();
   void code_constants();
   void code_class_nametab();
   void code_protoobj();
   void code_int_protobj();
   void code_string_protobj();
   int count_attr_num(CgenNodeP nd);
   void code_dispatchtab();
   void build_methodtab();
   void code_init();
   void code_objtab();
   void code_objtab_acc(List<CgenNode> *nds);

// The following creates an inheritance graph from
// a list of classes.  The graph is implemented as
// a tree of `CgenNode', and class names are placed
// in the base class symbol table.

   void install_basic_classes();
   void install_class(CgenNodeP nd);
   void install_classes(Classes cs);
   void build_inheritance_tree();
   void set_relations(CgenNodeP nd);
   void build_feature_tab();
   List<CgenNode> *list_enqueue(List<CgenNode> *l, CgenNodeP nd);
public:
   CgenClassTable(Classes, ostream& str);
   void code();
   CgenNodeP root();
   void code_method();
   int get_method_offset(Symbol class_name, Symbol method_name);
   List<Case_class> *sort(List<Case_class> *node_list);
   List<Case_class> *remove_entry(List<Case_class> *node_list, Case node);
   void code_typcase(Symbol node, int label);
};


class CgenNode : public class__class {
private: 
   CgenNodeP parentnd;                        // Parent of class
   List<CgenNode> *children;                  // Children of class
   Basicness basic_status;                    // `Basic' if class is basic
                                              // `NotBasic' otherwise
   int class_tag;
   List<Entry> *attr_list;
   List<Entry> *method_list;
   SymbolTable<Symbol, Entry> *method_symbtab;
   List<Entry> *let_list;
   List<Entry> *formal_list;

public:
   CgenNode(Class_ c,
            Basicness bstatus,
            CgenClassTableP class_table);

   void add_child(CgenNodeP child);
   List<CgenNode> *get_children() { return children; }
   void set_parentnd(CgenNodeP p);
   CgenNodeP get_parentnd() { return parentnd; }
   int basic() { return (basic_status == Basic); }
   void build_feature_tab();
   List<Entry> *add_attr(List<Entry> *feature_list, int flag);
   List<Entry> *add_method(List<Entry> *feature_list, SymbolTable<Symbol, Entry> *feature_symbtab, int flag);
   List<Entry> *add_attr_acc(List<Entry> *feature_list, Feature curr_feature);
   List<Entry> *add_method_acc(List<Entry> *feature_list, SymbolTable<Symbol, Entry> *feature_symbtab, Feature curr_feature);
   List<Entry> *list_enqueue(List<Entry> *orig_list, Symbol s);
   List<Entry> *build_attr_tab_acc(List<Entry> *feature_list, int flag);
   List<Entry> *build_method_tab_acc(List<Entry> *feature_list, SymbolTable<Symbol, Entry> *feature_symbtab, int flag);
   int get_class_tag() { return class_tag; }
   void set_class_tag(int t) { class_tag = t; }
   void code_method_tab(ostream& s);
   int get_attr_num() { return list_length(attr_list); }
   void code_init(ostream &s, CgenClassTableP curr_classtable, int &label_index);
   void code_objtab(ostream &s);
   void code_class_protobj(ostream &s);
   int get_attr_offset(Symbol attr_name);
   void code_method_name(ostream &s, Symbol method_name);
   int build_class_tag(ostream &s, int index);
   List<CgenNode> *list_enqueue(List<CgenNode> *l, CgenNodeP nd);
   void add_let_id(Symbol name);
   void remove_let_id();
   int get_let_offset(Symbol name);
   void add_formal_id(Symbol name);
   void empty_formal_id();
   int get_formal_offset(Symbol name);
   void code_method(ostream &s, CgenClassTableP curr_classtable, int &label_index);
   int get_method_offset(Symbol method_name);
   int find_max_child();
};

class BoolConst 
{
 private: 
  int val;
 public:
  BoolConst(int);
  void code_def(ostream&, int boolclasstag);
  void code_ref(ostream&) const;
};

