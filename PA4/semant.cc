

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "semant.h"
#include "utilities.h"


extern int semant_debug;
extern char *curr_filename;

//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
static Symbol
    arg,
    arg2,
    Bool,
    concat,
    cool_abort,
    copy,
    Int,
    in_int,
    in_string,
    IO,
    length,
    Main,
    main_meth,
    No_class,
    No_type,
    Object,
    out_int,
    out_string,
    prim_slot,
    self,
    SELF_TYPE,
    Str,
    str_field,
    substr,
    type_name,
    val;
//
// Initializing the predefined symbols.
//
static void initialize_constants(void)
{
    arg         = idtable.add_string("arg");
    arg2        = idtable.add_string("arg2");
    Bool        = idtable.add_string("Bool");
    concat      = idtable.add_string("concat");
    cool_abort  = idtable.add_string("abort");
    copy        = idtable.add_string("copy");
    Int         = idtable.add_string("Int");
    in_int      = idtable.add_string("in_int");
    in_string   = idtable.add_string("in_string");
    IO          = idtable.add_string("IO");
    length      = idtable.add_string("length");
    Main        = idtable.add_string("Main");
    main_meth   = idtable.add_string("main");
    //   _no_class is a symbol that can't be the name of any
    //   user-defined class.
    No_class    = idtable.add_string("_no_class");
    No_type     = idtable.add_string("_no_type");
    Object      = idtable.add_string("Object");
    out_int     = idtable.add_string("out_int");
    out_string  = idtable.add_string("out_string");
    prim_slot   = idtable.add_string("_prim_slot");
    self        = idtable.add_string("self");
    SELF_TYPE   = idtable.add_string("SELF_TYPE");
    Str         = idtable.add_string("String");
    str_field   = idtable.add_string("_str_field");
    substr      = idtable.add_string("substr");
    type_name   = idtable.add_string("type_name");
    val         = idtable.add_string("_val");
}

//Codes added
Symbol class__class::get_name() { return name; }
Symbol class__class::get_parent() { return parent; }
Features class__class::get_features() { return features; }
Symbol attr_class::get_name() { return name; }
Symbol attr_class::get_type() { return type_decl; }
Formals attr_class::get_formals() { return NULL; }
bool attr_class::attr_redefine_error(SymbolTable<Symbol, Entry> *curr_attr_declarations) {
    return (curr_attr_declarations->lookup(name) != NULL);
}
int attr_class::method_redefine_error(SymbolTable<Symbol, Feature_class> *curr_method_declarations, Symbol filename, ClassTable *curr_classtable) { return 0; }
void attr_class::add_attr(SymbolTable<Symbol, Entry> *curr_attr_declarations) {
    curr_attr_declarations->addid(name, type_decl);
}
void attr_class::add_method(SymbolTable<Symbol, Feature_class> *curr_method_declarations) {}
bool attr_class::self_error() { return name == self; }
bool method_class::self_error() { return false; }
int method_class::method_redefine_error(SymbolTable<Symbol, Feature_class> *curr_method_declarations, Symbol filename, ClassTable *curr_classtable) {
    if (curr_method_declarations->probe(name) != NULL)
        return 1;
    Formal curr_formal;
    for(int i = formals->first(); formals->more(i); i = formals->next(i)) {
        curr_formal = formals->nth(i);
        if (curr_formal->get_name() == self)
            return 7;        
        if (curr_formal->get_type() == SELF_TYPE)
            return 6;
        if (curr_classtable->no_type_error(filename, curr_formal->get_type(), curr_formal))
            return 2;
    }
    Feature predefined_method = curr_method_declarations->lookup(name);
    if (predefined_method == NULL) {
        return 0;
    }
    if (predefined_method->get_type() != return_type)
        return 3;
    Formals predefined_formals = predefined_method->get_formals();
    Formal predefined_formal;
    int i;
    for(i = formals->first(); formals->more(i); i = formals->next(i)) {
        if (!predefined_formals->more(i))
            return 4;
        curr_formal = formals->nth(i);
        predefined_formal = predefined_formals->nth(i);
        if (curr_formal->get_type() != predefined_formal->get_type())
            return 5;
    }
    if (predefined_formals->more(i))
        return 4;
    return 0;
}
bool method_class::attr_redefine_error(SymbolTable<Symbol, Entry> *curr_attr_declarations) { return false; }
void method_class::add_method(SymbolTable<Symbol, Feature_class> *curr_method_declarations) {
    curr_method_declarations->addid(name, this);
}
void method_class::add_attr(SymbolTable<Symbol, Entry> *curr_attr_declarations) {}
Symbol method_class::get_name() { return name; }
Symbol method_class::get_type() { return return_type; }
Formals method_class::get_formals() { return formals; }
Symbol formal_class::get_name() { return name; }
Symbol formal_class::get_type() { return type_decl; }
Symbol branch_class::get_name() { return name; }
Symbol branch_class::get_type() { return type_decl; }
Expression branch_class::get_expr() { return expr; }

void ClassTable::halt() {
    if (errors()) {
	    cerr << "Compilation halted due to static semantic errors." << endl;
	    exit(1);
    }
}
SymbolTable<Symbol, Class__class> *ClassTable::get_all_classes() { return classes_scope; }
bool ClassTable::is_subclass(Symbol name1, Symbol name2) {
    Class_ curr_class = classes_scope->lookup(name1);
    if (curr_class == NULL)
        return false;
    if (name1 == name2)
        return true;
    return (is_subclass(curr_class->get_parent(), name2));
}

bool ClassTable::no_type_error(Symbol filename, Symbol curr_type, tree_node *t) {
    if (curr_type != SELF_TYPE && curr_type != prim_slot && classes_scope->lookup(curr_type) == NULL) {
        semant_error(filename, t) << "Type " << curr_type << " is not defined.\n";
        return true;
    }
    return false;
}

Symbol ClassTable::lub(Symbol T1, Symbol T2) {
    if (is_subclass(T1, T2))
        return T2;
    if (is_subclass(T2, T1))
        return T1;
    return lub(classes_scope->lookup(T1)->get_parent(), T2);
}

bool ClassTable::no_method_error(Symbol class_name, Symbol method_name, Symbol filename, Expression curr_expression) {
    if (method_declarations->lookup(class_name)->lookup(method_name) == NULL) {
        semant_error(filename, curr_expression) << "Method " << method_name << " is not defined in class " << class_name << ".\n";
        return true;
    }
    return false;
}

Feature ClassTable::get_method_entity(Symbol class_name, Symbol method_name) {
    return method_declarations->lookup(class_name)->lookup(method_name);
}

ClassTable::ClassTable(Classes classes) : semant_errors(0) , error_stream(cerr) {

    /* Fill this in */
    install_basic_classes();
    all_classes = append_Classes(all_classes, classes);
    class_num = all_classes->len();
    class_names = new SymbolTable<Symbol, int>();
    class_names->enterscope();
    inheritance = new SymbolTable<int, int>();
    inheritance->enterscope();
    build_inheritance_graph();
    if (isCyclic())
        semant_error() << "The inheritance graph is not acyclic.\n";
}

void ClassTable::build_inheritance_graph() {
    classes_scope = new SymbolTable<Symbol, Class__class>();
    classes_scope->enterscope();
    Class_ class_entity;
    Symbol class_name;
    int j = 0;
    class_names->addid(No_class, new int(j++));
    for (int i = all_classes->first(); all_classes->more(i); i = all_classes->next(i)) {
        class_entity = all_classes->nth(i);
        class_name = class_entity->get_name();
        if (classes_scope->lookup(class_name) != NULL)
            semant_error(class_entity) << "Class " << class_name << " is defined multiple times.\n";
        else {
            classes_scope->addid(class_name, class_entity);
            class_names->addid(class_name, new int(j++));
        }
    }
    if (classes_scope->lookup(Main) == NULL)
        semant_error(class_entity) << "Class " << Main << " is not defined.\n"; 
    j = 1;
    int *parent_id;
    for (int i = all_classes->first(); all_classes->more(i); i = all_classes->next(i)) {
        parent_id = class_names->lookup(all_classes->nth(i)->get_parent());
        if (parent_id == NULL) {
            Class_ curr_class = all_classes->nth(i);
            semant_error(curr_class) << "Class " << curr_class->get_name() << "\'s parent class " << curr_class->get_parent() << " is not defined.\n";
        }
        else {
            inheritance->addid(j++, parent_id);
        }
    }
}

bool ClassTable::isCyclicUtil(int v, bool visited[], bool *recStack) {
    int *parent_id;
    if(visited[v] == false) {
        visited[v] = true;
        recStack[v] = true;
        parent_id = inheritance->lookup(v);
        if (parent_id != NULL) {
            if (!visited[*parent_id] && isCyclicUtil(*parent_id, visited, recStack))
                return true;
            else if (recStack[*parent_id])
                return true;
        }
    }
    recStack[v] = false;
    return false;
}

bool ClassTable::isCyclic() {
    bool *visited = new bool[class_num];
    bool *recStack = new bool[class_num];
    for(int i = 0; i < class_num; i++) {
        visited[i] = false;
        recStack[i] = false;
    }
    for(int i = 0; i < class_num; i++)
        if (isCyclicUtil(i, visited, recStack))
            return true;
    return false;
}

void ClassTable::install_basic_classes() {

    // The tree package uses these globals to annotate the classes built below.
   // curr_lineno  = 0;
    Symbol filename = stringtable.add_string("<basic class>");

    // The following demonstrates how to create dummy parse trees to
    // refer to basic Cool classes.  There's no need for method
    // bodies -- these are already built into the runtime system.

    // IMPORTANT: The results of the following expressions are
    // stored in local variables.  You will want to do something
    // with those variables at the end of this method to make this
    // code meaningful.

    //
    // The Object class has no parent class. Its methods are
    //        abort() : Object    aborts the program
    //        type_name() : Str   returns a string representation of class name
    //        copy() : SELF_TYPE  returns a copy of the object
    //
    // There is no need for method bodies in the basic classes---these
    // are already built in to the runtime system.

    Class_ Object_class =
	class_(Object,
	       No_class,
	       append_Features(
			       append_Features(
					       single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
					       single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
			       single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
	       filename);

    //
    // The IO class inherits from Object. Its methods are
    //        out_string(Str) : SELF_TYPE       writes a string to the output
    //        out_int(Int) : SELF_TYPE            "    an int    "  "     "
    //        in_string() : Str                 reads a string from the input
    //        in_int() : Int                      "   an int     "  "     "
    //
    Class_ IO_class =
	class_(IO,
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       single_Features(method(out_string, single_Formals(formal(arg, Str)),
										      SELF_TYPE, no_expr())),
							       single_Features(method(out_int, single_Formals(formal(arg, Int)),
										      SELF_TYPE, no_expr()))),
					       single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
			       single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
	       filename);

    //
    // The Int class has no methods and only a single attribute, the
    // "val" for the integer.
    //
    Class_ Int_class =
	class_(Int,
	       Object,
	       single_Features(attr(val, prim_slot, no_expr())),
	       filename);

    //
    // Bool also has only the "val" slot.
    //
    Class_ Bool_class =
	class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename);

    //
    // The class Str has a number of slots and operations:
    //       val                                  the length of the string
    //       str_field                            the string itself
    //       length() : Int                       returns length of the string
    //       concat(arg: Str) : Str               performs string concatenation
    //       substr(arg: Int, arg2: Int): Str     substring selection
    //
    Class_ Str_class =
	class_(Str,
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       append_Features(
									       single_Features(attr(val, Int, no_expr())),
									       single_Features(attr(str_field, prim_slot, no_expr()))),
							       single_Features(method(length, nil_Formals(), Int, no_expr()))),
					       single_Features(method(concat,
								      single_Formals(formal(arg, Str)),
								      Str,
								      no_expr()))),
			       single_Features(method(substr,
						      append_Formals(single_Formals(formal(arg, Int)),
								     single_Formals(formal(arg2, Int))),
						      Str,
						      no_expr()))),
	       filename);
      // Codes added
      all_classes = append_Classes(
                          single_Classes(Object_class),
                          append_Classes(
                              single_Classes(IO_class),
                              append_Classes(
                                  single_Classes(Int_class),
                                  append_Classes(
                                      single_Classes(Bool_class),
                                      single_Classes(Str_class)))));
}

// Codes added
void ClassTable::naming_scoping() {
    attr_declarations = new SymbolTable<Symbol, SymbolTable<Symbol, Entry> >();
    attr_declarations->enterscope();
    method_declarations = new SymbolTable<Symbol, SymbolTable<Symbol, Feature_class> >();
    method_declarations->enterscope();
    for (int j = all_classes->first(); all_classes->more(j); j = all_classes->next(j))
        naming_scoping(all_classes->nth(j));
}

void ClassTable::naming_scoping_helper(Class_ curr_class, SymbolTable<Symbol, Entry> *curr_attr_declarations, SymbolTable<Symbol, Feature_class> *curr_method_declarations, bool error_flag) {
    Features features = curr_class->get_features();
    Feature curr_feature;
    Symbol curr_type;
    for(int i = features->first(); features->more(i); i = features->next(i)) {
        curr_feature = features->nth(i);
        if (curr_feature->self_error())
            semant_error(curr_class->get_filename(), curr_feature) << "self is not allowed to appear in an attribute.\n";
        curr_type = curr_feature->get_type();
        if (!no_type_error(curr_class->get_filename(), curr_type, curr_feature)) {
            naming_scoping(curr_feature, curr_class->get_filename(), curr_attr_declarations, curr_method_declarations, error_flag);
        }
    }
}

void ClassTable::naming_scoping_acc(Class_ curr_class, SymbolTable<Symbol, Entry> *curr_attr_declarations, SymbolTable<Symbol, Feature_class> *curr_method_declarations) {
    Class_ parent_class;
    Symbol name = curr_class->get_name();
    if (name != Object) {
        parent_class = classes_scope->lookup(curr_class->get_parent());
        naming_scoping_acc(parent_class, curr_attr_declarations, curr_method_declarations);
    }
    else {
        curr_attr_declarations->enterscope();
        curr_method_declarations->enterscope();
    }
    naming_scoping_helper(curr_class, curr_attr_declarations, curr_method_declarations, false);
}

void ClassTable::naming_scoping(Class_ curr_class) {
    Symbol name = curr_class->get_name();
    SymbolTable<Symbol, Entry> *curr_attr_declarations = new SymbolTable<Symbol, Entry>();
    SymbolTable<Symbol, Feature_class> *curr_method_declarations = new SymbolTable<Symbol, Feature_class>();
    Class_ parent_class;
    Symbol parent_name;
    if (name != Object) {
        parent_class = classes_scope->lookup(curr_class->get_parent());
        parent_name = parent_class->get_name();
        if (parent_name == Int || parent_name == Str || parent_name == Bool)
            semant_error(curr_class->get_filename(), curr_class) << "Basic classes: Int, String, Bool cannot be inherited.\n";
        naming_scoping_acc(parent_class, curr_attr_declarations, curr_method_declarations);
    }
    curr_attr_declarations->enterscope();
    curr_method_declarations->enterscope();
    naming_scoping_helper(curr_class, curr_attr_declarations, curr_method_declarations, true);
    curr_attr_declarations->addid(self, name);
    if (name == Main) {
        if (curr_method_declarations->probe(main_meth) == NULL)
            semant_error(curr_class->get_filename(), curr_class) << "Method main is not defined.\n";
        else if (curr_method_declarations->probe(main_meth)->get_formals()->len() != 0)
            semant_error(curr_class->get_filename(), curr_class) << "Method main should take no formal parameters.\n";
    }
    attr_declarations->addid(name, curr_attr_declarations);
    method_declarations->addid(name, curr_method_declarations);
}

void ClassTable::naming_scoping(Feature curr_feature, Symbol filename, SymbolTable<Symbol, Entry> *curr_attr_declarations, SymbolTable<Symbol, Feature_class> *curr_method_declarations, bool error_flag) {
    Symbol name = curr_feature->get_name();
    if (error_flag && curr_feature->attr_redefine_error(curr_attr_declarations)) {
        semant_error(filename, curr_feature) << "Attribute " << name << " is defined multiple times.\n";
    }
    else {
        curr_feature->add_attr(curr_attr_declarations);
    }
    if (error_flag) {
        switch (curr_feature->method_redefine_error(curr_method_declarations, filename, this)) {
            case 1:
                semant_error(filename, curr_feature) << "Method " << name << " is redefined in the current class.\n";
                break;
            case 2:
                break;
            case 3:
                semant_error(filename, curr_feature) << "Method " << name << " is redefined with different return types.\n";
                break;
            case 4:
                semant_error(filename, curr_feature) << "Method " << name << " is redefined with different number of formals.\n";
                break;            
            case 5:
                semant_error(filename, curr_feature) << "Method " << name << " is redefined with different formal types.\n";
                break;
            case 6:
                semant_error(filename, curr_feature) << "SELF_TYPE is not allowed to appear in a formal binding.\n";
                break;
            case 7:
                semant_error(filename, curr_feature) << "self is not allowed to appear in a formal binding.\n";
                break;
            case 0:
                curr_feature->add_method(curr_method_declarations);
                break;
        }
    }
    else
        curr_feature->add_method(curr_method_declarations);
}

void ClassTable::type_checking() {
    Symbol name;
    Class_ curr_class;
    for (int i = all_classes->first(); all_classes->more(i); i = all_classes->next(i)) {
        curr_class = all_classes->nth(i);
        name = curr_class->get_name();
        curr_class->type_checking(attr_declarations->lookup(name), method_declarations->lookup(name), this);
    }
}

void class__class::type_checking(SymbolTable<Symbol, Entry> *attr_declarations, SymbolTable<Symbol, Feature_class> *method_declarations, ClassTable *curr_classtable) {
    for(int i = features->first(); features->more(i); i = features->next(i)) {
        features->nth(i)->type_checking(attr_declarations, method_declarations, curr_classtable, this);
    }
    Symbol parent_name = get_parent();
    Class_ curr_class;
    Features curr_features;
    Feature curr_feature;
    while (parent_name != No_class) {
        curr_class = curr_classtable->get_all_classes()->lookup(parent_name);
        curr_features = curr_class->get_features();
        for(int i = curr_features->first(); curr_features->more(i); i = curr_features->next(i)) {
            curr_feature = curr_features->nth(i);
            if (attr_declarations->probe(curr_feature->get_name()) == NULL && method_declarations->probe(curr_feature->get_name()) == NULL)
                curr_feature->type_checking(attr_declarations, method_declarations, curr_classtable, this);
        }
        parent_name = curr_class->get_parent();
    }
}

void attr_class::type_checking(SymbolTable<Symbol, Entry> *attr_declarations, SymbolTable<Symbol, Feature_class> *method_declarations, ClassTable *curr_classtable, Class_ curr_class) {
    init = init->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);
    Symbol init_type = init->get_type();
    if (init_type != No_type){
        Symbol real_type = type_decl;
        if (type_decl == SELF_TYPE)
            real_type = attr_declarations->lookup(self);
        if (init_type == SELF_TYPE)
            init_type = attr_declarations->lookup(self);
        if (!curr_classtable->is_subclass(init_type, real_type))
            curr_classtable->semant_error(curr_class->get_filename(), this) << "Identifier " << name << " declared type " << real_type << " but assigned type " << init_type << " .\n";
    }
    else
        init = init->set_type(type_decl);
}

void method_class::type_checking(SymbolTable<Symbol, Entry> *attr_declarations, SymbolTable<Symbol, Feature_class> *method_declarations, ClassTable *curr_classtable, Class_ curr_class) {
    attr_declarations->enterscope();
    Symbol formal_type;
    for(int i = formals->first(); formals->more(i); i = formals->next(i)) {
        formal_type = formals->nth(i)->get_type();
        attr_declarations->addid(formals->nth(i)->get_name(), formal_type);
    }
    expr = expr->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);
    Symbol expr_type = expr->get_type();
    Symbol real_type = return_type;
    if (expr_type != No_type){
        if (return_type == SELF_TYPE)
            real_type = attr_declarations->lookup(self);
        if (expr_type == SELF_TYPE)
            expr_type = attr_declarations->lookup(self);
        if (!curr_classtable->is_subclass(expr_type, real_type))
            curr_classtable->semant_error(curr_class->get_filename(), this) << "The declared return type of method " << name << " is " << real_type << " but the type of the method body is " << expr_type << " .\n";
        attr_declarations->exitscope();
    }
}

Expression assign_class::type_checking(SymbolTable<Symbol, Entry> *attr_declarations, SymbolTable<Symbol, Feature_class> *method_declarations, ClassTable *curr_classtable, Class_ curr_class) {
    expr = expr->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);
    Symbol expr_type = expr->get_type();
    Symbol type_decl = attr_declarations->lookup(name);
    if (curr_classtable->is_subclass(expr_type, type_decl)) {
        return set_type(expr_type);
    }
    else {
        curr_classtable->semant_error(curr_class->get_filename(), this) << "Identifier " << name << " declared type " << type_decl << " but assigned type " << expr_type << " .\n";
        return set_type(Object);
    }
}

Expression static_dispatch_class::type_checking(SymbolTable<Symbol, Entry> *attr_declarations, SymbolTable<Symbol, Feature_class> *method_declarations, ClassTable *curr_classtable, Class_ curr_class) {
    if (curr_classtable->no_type_error(curr_class->get_filename(), type_name, this))
        return set_type(Object);
    expr = expr->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);
    Symbol expr_type = expr->get_type();
    if (expr_type == SELF_TYPE)
        expr_type = attr_declarations->lookup(self);
    if (!curr_classtable->is_subclass(expr_type, type_name))
        curr_classtable->semant_error(curr_class->get_filename(), this) << "The caller is not of type " << type_name << ".\n";
    if (curr_classtable->no_method_error(type_name, name, curr_class->get_filename(), this))
        return set_type(Object);
    Feature method_entity = curr_classtable->get_method_entity(type_name, name);
    Expression expr_with_type;
    Expressions new_actual;
    for (int i = actual->first(); actual->more(i); i = actual->next(i)) {
        if (!method_entity->get_formals()->more(i)) {
            curr_classtable->semant_error(curr_class->get_filename(), this) << "Number of parameters is not the same of declared.\n";
            return set_type(Object);
        }
        expr_with_type = actual->nth(i)->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);
        if (!curr_classtable->is_subclass(expr_with_type->get_type(), method_entity->get_formals()->nth(i)->get_type())) {
            curr_classtable->semant_error(curr_class->get_filename(), this) << "Parameters are not of the types declared.\n";
            expr_with_type = expr_with_type->set_type(Object);
        }
        if (i == actual->first())
            new_actual = single_Expressions(expr_with_type);
        else
            new_actual = append_Expressions(new_actual, single_Expressions(expr_with_type));
    }
    actual = new_actual;
    Symbol return_type = method_entity->get_type();
    if (return_type == SELF_TYPE)
        return set_type(expr_type);
    else
        return set_type(return_type);
}

Expression dispatch_class::type_checking(SymbolTable<Symbol, Entry> *attr_declarations, SymbolTable<Symbol, Feature_class> *method_declarations, ClassTable *curr_classtable, Class_ curr_class) {
    expr = expr->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);
    Symbol expr_type = expr->get_type();
    if (expr_type == SELF_TYPE)
        expr_type = attr_declarations->lookup(self);
    if (curr_classtable->no_method_error(expr_type, name, curr_class->get_filename(), this))
        return set_type(Object);
    Feature method_entity = curr_classtable->get_method_entity(expr_type, name);
    Expression expr_with_type;
    Expressions new_actual = nil_Expressions();
    int i;
    for (i = actual->first(); actual->more(i); i = actual->next(i)) {
        if (!method_entity->get_formals()->more(i)) {
            curr_classtable->semant_error(curr_class->get_filename(), this) << "Number of parameters is not the same of declared.\n";
            return set_type(Object);
        }
        expr_with_type = actual->nth(i)->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);
        if (!curr_classtable->is_subclass(expr_with_type->get_type(), method_entity->get_formals()->nth(i)->get_type())) {
            curr_classtable->semant_error(curr_class->get_filename(), this) << "Parameters are not of the types declared.\n";
            expr_with_type = expr_with_type->set_type(Object);
        }
        if (i == actual->first())
            new_actual = single_Expressions(expr_with_type);
        else
            new_actual = append_Expressions(new_actual, single_Expressions(expr_with_type));
    }
    if (method_entity->get_formals()->more(i)) {
        curr_classtable->semant_error(curr_class->get_filename(), this) << "Number of parameters is not the same of declared.\n";
        return set_type(Object);
    }
    actual = new_actual;
    Symbol return_type = method_entity->get_type();
    if (return_type == SELF_TYPE)
        return set_type(expr_type);
    else
        return set_type(return_type);
}

Expression cond_class::type_checking(SymbolTable<Symbol, Entry> *attr_declarations, SymbolTable<Symbol, Feature_class> *method_declarations, ClassTable *curr_classtable, Class_ curr_class) {
    pred = pred->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);
    if (pred->get_type() != Bool) {
        curr_classtable->semant_error(curr_class->get_filename(), this) << "Predicate is not of type Bool.\n";
        return set_type(Object);
    }
    then_exp = then_exp->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);
    else_exp = else_exp->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);   
    return set_type(curr_classtable->lub(then_exp->get_type(), else_exp->get_type()));
}

Expression loop_class::type_checking(SymbolTable<Symbol, Entry> *attr_declarations, SymbolTable<Symbol, Feature_class> *method_declarations, ClassTable *curr_classtable, Class_ curr_class) {
    pred = pred->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);
    if (pred->get_type() != Bool) {
        curr_classtable->semant_error(curr_class->get_filename(), this) << "Predicate is not of type Bool.\n";
    }
    body = body->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);
    return set_type(Object);
}

Expression typcase_class::type_checking(SymbolTable<Symbol, Entry> *attr_declarations, SymbolTable<Symbol, Feature_class> *method_declarations, ClassTable *curr_classtable, Class_ curr_class) {
    expr = expr->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);
    attr_declarations->enterscope();
    Expression expr_with_type;
    Symbol curr_type;
    Cases new_cases;
    Symbol final_type;
    List<Entry> *type_list = NULL;
    for (int i = cases->first(); cases->more(i); i = cases->next(i)) {
        Case curr_case = cases->nth(i);
        if (curr_case->get_name() == self) {
            curr_classtable->semant_error(curr_class->get_filename(), this) << "self is not allowed to appear in a case binding.\n";
            return set_type(Object);
        }
        curr_type = curr_case->get_type();
        for (List<Entry> *l = type_list; l; l = l->tl()) {
            if (l->hd() == curr_type) {
                curr_classtable->semant_error(curr_class->get_filename(), this) << "Type " << curr_type << " appears multiple times.\n";
                return set_type(Object);
            }
        }
        type_list = new List<Entry>(curr_type, type_list);
        if (curr_classtable->no_type_error(curr_class->get_filename(), curr_type, curr_case))
            expr_with_type = curr_case->get_expr()->set_type(Object);
        else {
            attr_declarations->addid(curr_case->get_name(), curr_type);
            expr_with_type = curr_case->get_expr()->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);
        }
        if (i == cases->first()) {
            final_type = expr_with_type->get_type();
            new_cases = single_Cases(branch(curr_case->get_name(), curr_type, expr_with_type));
        }
        else {
            final_type = curr_classtable->lub(final_type, expr_with_type->get_type());
            new_cases = append_Cases(new_cases, single_Cases(branch(curr_case->get_name(), curr_type, expr_with_type)));
        }
    }
    cases = new_cases;
    return set_type(final_type);
}

Expression block_class::type_checking(SymbolTable<Symbol, Entry> *attr_declarations, SymbolTable<Symbol, Feature_class> *method_declarations, ClassTable *curr_classtable, Class_ curr_class) {
    Expressions new_body;
    Expression expr_with_type;
    for (int i = body->first(); body->more(i); i = body->next(i)) {
        expr_with_type = body->nth(i)->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);
        if (i == body->first())
            new_body = single_Expressions(expr_with_type);
        else
            new_body = append_Expressions(new_body, single_Expressions(expr_with_type));
        if (!body->more(body->next(i)))
            return set_type(expr_with_type->get_type());
    }
}

Expression plus_class::type_checking(SymbolTable<Symbol, Entry> *attr_declarations, SymbolTable<Symbol, Feature_class> *method_declarations, ClassTable *curr_classtable, Class_ curr_class) {
    e1 = e1->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);
    if (e1->get_type() != Int) {
        curr_classtable->semant_error(curr_class->get_filename(), this) << "Left part of + is not of type Int.\n";
        return set_type(Object);
    }
    e2 = e2->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);
    if (e2->get_type() != Int) {
        curr_classtable->semant_error(curr_class->get_filename(), this) << "Right part of + is not of type Int.\n";
        return set_type(Object);
    }
    return set_type(Int);
}

Expression sub_class::type_checking(SymbolTable<Symbol, Entry> *attr_declarations, SymbolTable<Symbol, Feature_class> *method_declarations, ClassTable *curr_classtable, Class_ curr_class) {
    e1 = e1->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);
    if (e1->get_type() != Int) {
        curr_classtable->semant_error(curr_class->get_filename(), this) << "Left part of - is not of type Int.\n";
        return set_type(Object);
    }
    e2 = e2->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);
    if (e2->get_type() != Int) {
        curr_classtable->semant_error(curr_class->get_filename(), this) << "Right part of - is not of type Int.\n";
        return set_type(Object);
    }
    return set_type(Int);
}

Expression mul_class::type_checking(SymbolTable<Symbol, Entry> *attr_declarations, SymbolTable<Symbol, Feature_class> *method_declarations, ClassTable *curr_classtable, Class_ curr_class) {
    e1 = e1->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);
    if (e1->get_type() != Int) {
        curr_classtable->semant_error(curr_class->get_filename(), this) << "Left part of * is not of type Int.\n";
        return set_type(Object);
    }
    e2 = e2->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);
    if (e2->get_type() != Int) {
        curr_classtable->semant_error(curr_class->get_filename(), this) << "Right part of * is not of type Int.\n";
        return set_type(Object);
    }
    return set_type(Int);
}

Expression divide_class::type_checking(SymbolTable<Symbol, Entry> *attr_declarations, SymbolTable<Symbol, Feature_class> *method_declarations, ClassTable *curr_classtable, Class_ curr_class) {
    e1 = e1->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);
    if (e1->get_type() != Int) {
        curr_classtable->semant_error(curr_class->get_filename(), this) << "Left part of / is not of type Int.\n";
        return set_type(Object);
    }
    e2 = e2->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);
    if (e2->get_type() != Int) {
        curr_classtable->semant_error(curr_class->get_filename(), this) << "Right part of / is not of type Int.\n";
        return set_type(Object);
    }
    return set_type(Int);
}

Expression neg_class::type_checking(SymbolTable<Symbol, Entry> *attr_declarations, SymbolTable<Symbol, Feature_class> *method_declarations, ClassTable *curr_classtable, Class_ curr_class) {
    e1 = e1->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);
    if (e1->get_type() != Int) {
        curr_classtable->semant_error(curr_class->get_filename(), this) << "Right part of ~ is not of type Int.\n";
        return set_type(Object);
    }
    return set_type(Int);
}

Expression lt_class::type_checking(SymbolTable<Symbol, Entry> *attr_declarations, SymbolTable<Symbol, Feature_class> *method_declarations, ClassTable *curr_classtable, Class_ curr_class) {
    e1 = e1->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);
    if (e1->get_type() != Int) {
        curr_classtable->semant_error(curr_class->get_filename(), this) << "Left part of < is not of type Int.\n";
        return set_type(Object);
    }
    e2 = e2->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);
    if (e2->get_type() != Int) {
        curr_classtable->semant_error(curr_class->get_filename(), this) << "Right part of < is not of type Int.\n";
        return set_type(Object);
    }
    return set_type(Bool);
}

Expression eq_class::type_checking(SymbolTable<Symbol, Entry> *attr_declarations, SymbolTable<Symbol, Feature_class> *method_declarations, ClassTable *curr_classtable, Class_ curr_class) {
    e1 = e1->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);
    e2 = e2->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);
    if (e1->get_type() == Int) {
        if (e2->get_type() != Int) {
            curr_classtable->semant_error(curr_class->get_filename(), this) << "Right part of = is not of type Int.\n";
            return set_type(Object);
        }
        else
            return set_type(Bool);
    }
    else if (e1->get_type() == Bool) {
        if (e2->get_type() != Bool) {
            curr_classtable->semant_error(curr_class->get_filename(), this) << "Right part of = is not of type Bool.\n";
            return set_type(Object);
        }
        else
            return set_type(Bool);
    }
    else if (e1->get_type() == Str) {
        if (e2->get_type() != Str) {
            curr_classtable->semant_error(curr_class->get_filename(), this) << "Right part of = is not of type Str.\n";
            return set_type(Object);
        }
        else
            return set_type(Bool);
    }
    else {
        curr_classtable->semant_error(curr_class->get_filename(), this) << "Left part of = is not of type Int or Bool or String.\n";
        return set_type(Object);
    }

}

Expression leq_class::type_checking(SymbolTable<Symbol, Entry> *attr_declarations, SymbolTable<Symbol, Feature_class> *method_declarations, ClassTable *curr_classtable, Class_ curr_class) {
    e1 = e1->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);
    if (e1->get_type() != Int) {
        curr_classtable->semant_error(curr_class->get_filename(), this) << "Left part of <= is not of type Int.\n";
        return set_type(Object);
    }
    e2 = e2->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);
    if (e2->get_type() != Int) {
        curr_classtable->semant_error(curr_class->get_filename(), this) << "Right part of <= is not of type Int.\n";
        return set_type(Object);
    }
    return set_type(Bool);
}

Expression comp_class::type_checking(SymbolTable<Symbol, Entry> *attr_declarations, SymbolTable<Symbol, Feature_class> *method_declarations, ClassTable *curr_classtable, Class_ curr_class) {
    e1 = e1->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);
    if (e1->get_type() != Bool) {
        curr_classtable->semant_error(curr_class->get_filename(), this) << "Right part of \"not\" is not of type Int.\n";
        return set_type(Object);
    }
    return set_type(Bool);
}

Expression int_const_class::type_checking(SymbolTable<Symbol, Entry> *attr_declarations, SymbolTable<Symbol, Feature_class> *method_declarations, ClassTable *curr_classtable, Class_ curr_class) {
    return set_type(Int);
}

Expression bool_const_class::type_checking(SymbolTable<Symbol, Entry> *attr_declarations, SymbolTable<Symbol, Feature_class> *method_declarations, ClassTable *curr_classtable, Class_ curr_class) {
    return set_type(Bool);
}

Expression string_const_class::type_checking(SymbolTable<Symbol, Entry> *attr_declarations, SymbolTable<Symbol, Feature_class> *method_declarations, ClassTable *curr_classtable, Class_ curr_class) {
    return set_type(Str);
}

Expression new__class::type_checking(SymbolTable<Symbol, Entry> *attr_declarations, SymbolTable<Symbol, Feature_class> *method_declarations, ClassTable *curr_classtable, Class_ curr_class) {
    if (curr_classtable->no_type_error(curr_class->get_filename(), type_name, this)) 
        return set_type(Object);
    else 
        return set_type(type_name);
}

Expression isvoid_class::type_checking(SymbolTable<Symbol, Entry> *attr_declarations, SymbolTable<Symbol, Feature_class> *method_declarations, ClassTable *curr_classtable, Class_ curr_class) {
    return set_type(Bool);
}

Expression no_expr_class::type_checking(SymbolTable<Symbol, Entry> *attr_declarations, SymbolTable<Symbol, Feature_class> *method_declarations, ClassTable *curr_classtable, Class_ curr_class) {
    return set_type(No_type);
}

Expression object_class::type_checking(SymbolTable<Symbol, Entry> *attr_declarations, SymbolTable<Symbol, Feature_class> *method_declarations, ClassTable *curr_classtable, Class_ curr_class) {
    return set_type(attr_declarations->lookup(name));
}

Expression let_class::type_checking(SymbolTable<Symbol, Entry> *attr_declarations, SymbolTable<Symbol, Feature_class> *method_declarations, ClassTable *curr_classtable, Class_ curr_class) {
    if (identifier == self) {
        curr_classtable->semant_error(curr_class->get_filename(), this) << "self is not allowed to appear in a let binding.\n";
        return set_type(Object);
    }
    init = init->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);
    Symbol init_type = init->get_type();
    if (init_type != No_type){
        Symbol real_type = type_decl;
        if (type_decl == SELF_TYPE)
            real_type = attr_declarations->lookup(self);
        if (init_type == SELF_TYPE)
            init_type = attr_declarations->lookup(self);
        if (!curr_classtable->is_subclass(init_type, real_type))
            curr_classtable->semant_error(curr_class->get_filename(), this) << "Identifier " << identifier << " declared type " << real_type << " but assigned type " << init_type << " .\n";
    }
    else
        init = init->set_type(type_decl);
    attr_declarations->addid(identifier, type_decl);
    body = body->type_checking(attr_declarations, method_declarations, curr_classtable, curr_class);
    return set_type(body->get_type());
}

////////////////////////////////////////////////////////////////////
//
// semant_error is an overloaded function for reporting errors
// during semantic analysis.  There are three versions:
//
//    ostream& ClassTable::semant_error()
//
//    ostream& ClassTable::semant_error(Class_ c)
//       print line number and filename for `c'
//
//    ostream& ClassTable::semant_error(Symbol filename, tree_node *t)
//       print a line number and filename
//
///////////////////////////////////////////////////////////////////

ostream& ClassTable::semant_error(Class_ c)
{
    return semant_error(c->get_filename(),c);
}

ostream& ClassTable::semant_error(Symbol filename, tree_node *t)
{
    error_stream << filename << ":" << t->get_line_number() << ": ";
    return semant_error();
}

ostream& ClassTable::semant_error()
{
    semant_errors++;
    return error_stream;
}



/*   This is the entry point to the semantic checker.

     Your checker should do the following two things:

     1) Check that the program is semantically correct
     2) Decorate the abstract syntax tree with type information
        by setting the `type' field in each Expression node.
        (see `tree.h')

     You are free to first do 1), make sure you catch all semantic
     errors. Part 2) can be done in a second stage, when you want
     to build mycoolc.
 */
void program_class::semant()
{
    initialize_constants();

    /* ClassTable constructor may do some semantic analysis */
    ClassTable *classtable = new ClassTable(classes);
    classtable->halt();
    /* some semantic analysis code may go here */
    classtable->naming_scoping();
    classtable->type_checking();
    classtable->halt();
}
