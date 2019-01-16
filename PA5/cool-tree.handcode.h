//
// The following include files must come first.

#ifndef COOL_TREE_HANDCODE_H
#define COOL_TREE_HANDCODE_H

class CgenClassTable;
typedef CgenClassTable *CgenClassTableP;

class CgenNode;
typedef CgenNode *CgenNodeP;
#include <iostream>
#include "tree.h"
#include "cool.h"
#include "stringtab.h"
#define yylineno curr_lineno;
extern int yylineno;

inline Boolean copy_Boolean(Boolean b) {return b; }
inline void assert_Boolean(Boolean) {}
inline void dump_Boolean(ostream& stream, int padding, Boolean b)
	{ stream << pad(padding) << (int) b << "\n"; }

void dump_Symbol(ostream& stream, int padding, Symbol b);
void assert_Symbol(Symbol b);
Symbol copy_Symbol(Symbol b);

class Program_class;
typedef Program_class *Program;
class Class__class;
typedef Class__class *Class_;
class Feature_class;
typedef Feature_class *Feature;
class Formal_class;
typedef Formal_class *Formal;
class Expression_class;
typedef Expression_class *Expression;
class Case_class;
typedef Case_class *Case;

typedef list_node<Class_> Classes_class;
typedef Classes_class *Classes;
typedef list_node<Feature> Features_class;
typedef Features_class *Features;
typedef list_node<Formal> Formals_class;
typedef Formals_class *Formals;
typedef list_node<Expression> Expressions_class;
typedef Expressions_class *Expressions;
typedef list_node<Case> Cases_class;
typedef Cases_class *Cases;

#define Program_EXTRAS                          \
virtual void cgen(ostream&) = 0;		\
virtual void dump_with_types(ostream&, int) = 0; 



#define program_EXTRAS                          \
void cgen(ostream&);     			\
void dump_with_types(ostream&, int);            

#define Class__EXTRAS                   \
virtual Symbol get_name() = 0;  	\
virtual Symbol get_parent() = 0;    	\
virtual Symbol get_filename() = 0;      \
virtual Features get_features() = 0;	\
virtual void dump_with_types(ostream&,int) = 0; 


#define class__EXTRAS                                  \
Symbol get_name()   { return name; }		       \
Symbol get_parent() { return parent; }     	       \
Symbol get_filename() { return filename; }             \
Features get_features() { return features; }	\
void dump_with_types(ostream&,int);                    


#define Feature_EXTRAS                                        \
virtual bool attr_flag() = 0;	\
virtual Symbol get_name() = 0;	\
virtual Symbol get_type() = 0;	\
virtual char *method_string() = 0;	\
virtual void dump_with_types(ostream&,int) = 0;	\
virtual void code(ostream &s, CgenNodeP curr_node, CgenClassTableP curr_classtable, int &label_index) = 0;	\
virtual void code_method(ostream &s, CgenNodeP curr_node, CgenClassTableP curr_classtable, int &label_index) = 0;


#define Feature_SHARED_EXTRAS                                       \
void dump_with_types(ostream&,int);	\
Symbol get_name() { return name; }

#define attr_EXTRAS	\
bool attr_flag() { return true; }	\
Symbol get_type() { return type_decl; }	\
char *method_string() { return NULL; }	\
void code(ostream &s, CgenNodeP curr_node, CgenClassTableP curr_classtable, int &label_index) { init->code(s, curr_node, curr_classtable, label_index); }	\
void code_method(ostream &s, CgenNodeP curr_node, CgenClassTableP curr_classtable, int &label_index) {}

#define method_EXTRAS	\
bool attr_flag() { return false; }	\
Symbol get_type() { return return_type; }	\
char *method_string()	{ return name->get_string(); }	\
void code(ostream &s, CgenNodeP curr_node, CgenClassTableP curr_classtable, int &label_index) {}	\
void code_method(ostream &s, CgenNodeP curr_node, CgenClassTableP curr_classtable, int &label_index);

#define Formal_EXTRAS                              \
virtual void dump_with_types(ostream&,int) = 0;	\
virtual Symbol get_name() = 0;


#define formal_EXTRAS                           \
void dump_with_types(ostream&,int);	\
Symbol get_name() { return name; }


#define Case_EXTRAS                             \
virtual void dump_with_types(ostream& ,int) = 0;	\
virtual Symbol get_name() = 0;	\
virtual Symbol get_type() = 0;	\
virtual Expression get_expr() = 0;


#define branch_EXTRAS                                   \
void dump_with_types(ostream& ,int);	\
Symbol get_name() { return name; }	\
Symbol get_type() { return type_decl; }	\
Expression get_expr() { return expr; }


#define Expression_EXTRAS                    \
Symbol type;                                 \
Symbol get_type() { return type; }           \
Expression set_type(Symbol s) { type = s; return this; } \
virtual void code(ostream&, CgenNodeP, CgenClassTableP, int&) = 0; \
virtual void dump_with_types(ostream&,int) = 0;  \
void dump_type(ostream&, int);               \
Expression_class() { type = (Symbol) NULL; }

#define Expression_SHARED_EXTRAS           \
void code(ostream&, CgenNodeP, CgenClassTableP, int&); 			   \
void dump_with_types(ostream&,int); 

#endif
