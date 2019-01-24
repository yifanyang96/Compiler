
//**************************************************************
//
// Code generator SKELETON
//
// Read the comments carefully. Make sure to
//    initialize the base class tags in
//       `CgenClassTable::CgenClassTable'
//
//    Add the label for the dispatch tables to
//       `IntEntry::code_def'
//       `StringEntry::code_def'
//       `BoolConst::code_def'
//
//    Add code to emit everyting else that is needed
//       in `CgenClassTable::code'
//
//
// The files as provided will produce code to begin the code
// segments, declare globals, and emit constants.  You must
// fill in the rest.
//
//**************************************************************

#include "cgen.h"
#include "cgen_gc.h"

extern void emit_string_constant(ostream& str, char *s);
extern int cgen_debug;

//
// Three symbols from the semantic analyzer (semant.cc) are used.
// If e : No_type, then no code is generated for e.
// Special code is generated for new SELF_TYPE.
// The name "self" also generates code different from other references.
//
//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
Symbol 
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

static char *gc_init_names[] =
  { "_NoGC_Init", "_GenGC_Init", "_ScnGC_Init" };
static char *gc_collect_names[] =
  { "_NoGC_Collect", "_GenGC_Collect", "_ScnGC_Collect" };


//  BoolConst is a class that implements code generation for operations
//  on the two booleans, which are given global names here.
BoolConst falsebool(FALSE);
BoolConst truebool(TRUE);

//*********************************************************
//
// Define method for code generation
//
// This is the method called by the compiler driver
// `cgtest.cc'. cgen takes an `ostream' to which the assembly will be
// emmitted, and it passes this and the class list of the
// code generator tree to the constructor for `CgenClassTable'.
// That constructor performs all of the work of the code
// generator.
//
//*********************************************************

void program_class::cgen(ostream &os) 
{
  // spim wants comments to start with '#'
  os << "# start of generated code\n";

  initialize_constants();
  CgenClassTable *codegen_classtable = new CgenClassTable(classes,os);
  codegen_classtable->dump();

  os << "\n# end of generated code\n";
}


//////////////////////////////////////////////////////////////////////////////
//
//  emit_* procedures
//
//  emit_X  writes code for operation "X" to the output stream.
//  There is an emit_X for each opcode X, as well as emit_ functions
//  for generating names according to the naming conventions (see emit.h)
//  and calls to support functions defined in the trap handler.
//
//  Register names and addresses are passed as strings.  See `emit.h'
//  for symbolic names you can use to refer to the strings.
//
//////////////////////////////////////////////////////////////////////////////

static void emit_load(char *dest_reg, int offset, char *source_reg, ostream& s)
{
  s << LW << dest_reg << " " << offset * WORD_SIZE << "(" << source_reg << ")" 
    << endl;
}

static void emit_store(char *source_reg, int offset, char *dest_reg, ostream& s)
{
  s << SW << source_reg << " " << offset * WORD_SIZE << "(" << dest_reg << ")"
      << endl;
}

static void emit_load_imm(char *dest_reg, int val, ostream& s)
{ s << LI << dest_reg << " " << val << endl; }

static void emit_load_address(char *dest_reg, char *address, ostream& s)
{ s << LA << dest_reg << " " << address << endl; }

static void emit_partial_load_address(char *dest_reg, ostream& s)
{ s << LA << dest_reg << " "; }

static void emit_load_bool(char *dest, const BoolConst& b, ostream& s)
{
  emit_partial_load_address(dest,s);
  b.code_ref(s);
  s << endl;
}

static void emit_load_string(char *dest, StringEntry *str, ostream& s)
{
  emit_partial_load_address(dest,s);
  str->code_ref(s);
  s << endl;
}

static void emit_load_int(char *dest, IntEntry *i, ostream& s)
{
  emit_partial_load_address(dest,s);
  i->code_ref(s);
  s << endl;
}

static void emit_move(char *dest_reg, char *source_reg, ostream& s)
{ s << MOVE << dest_reg << " " << source_reg << endl; }

static void emit_neg(char *dest, char *src1, ostream& s)
{ s << NEG << dest << " " << src1 << endl; }

static void emit_add(char *dest, char *src1, char *src2, ostream& s)
{ s << ADD << dest << " " << src1 << " " << src2 << endl; }

static void emit_addu(char *dest, char *src1, char *src2, ostream& s)
{ s << ADDU << dest << " " << src1 << " " << src2 << endl; }

static void emit_addiu(char *dest, char *src1, int imm, ostream& s)
{ s << ADDIU << dest << " " << src1 << " " << imm << endl; }

static void emit_div(char *dest, char *src1, char *src2, ostream& s)
{ s << DIV << dest << " " << src1 << " " << src2 << endl; }

static void emit_mul(char *dest, char *src1, char *src2, ostream& s)
{ s << MUL << dest << " " << src1 << " " << src2 << endl; }

static void emit_sub(char *dest, char *src1, char *src2, ostream& s)
{ s << SUB << dest << " " << src1 << " " << src2 << endl; }

static void emit_sll(char *dest, char *src1, int num, ostream& s)
{ s << SLL << dest << " " << src1 << " " << num << endl; }

static void emit_jalr(char *dest, ostream& s)
{ s << JALR << "\t" << dest << endl; }

static void emit_jal(char *address,ostream &s)
{ s << JAL << address << endl; }

static void emit_return(ostream& s)
{ s << RET << endl; }

static void emit_gc_assign(ostream& s)
{ s << JAL << "_GenGC_Assign" << endl; }

static void emit_disptable_ref(Symbol sym, ostream& s)
{  s << sym << DISPTAB_SUFFIX; }

static void emit_init_ref(Symbol sym, ostream& s)
{ s << sym << CLASSINIT_SUFFIX; }

static void emit_label_ref(int l, ostream &s)
{ s << "label" << l; }

static void emit_protobj_ref(Symbol sym, ostream& s)
{ s << sym << PROTOBJ_SUFFIX; }

static void emit_method_ref(Symbol classname, Symbol methodname, ostream& s)
{ s << classname << METHOD_SEP << methodname; }

static void emit_label_def(int l, ostream &s)
{
  emit_label_ref(l,s);
  s << ":" << endl;
}

static void emit_beqz(char *source, int label, ostream &s)
{
  s << BEQZ << source << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_beq(char *src1, char *src2, int label, ostream &s)
{
  s << BEQ << src1 << " " << src2 << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_bne(char *src1, char *src2, int label, ostream &s)
{
  s << BNE << src1 << " " << src2 << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_bleq(char *src1, char *src2, int label, ostream &s)
{
  s << BLEQ << src1 << " " << src2 << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_blt(char *src1, char *src2, int label, ostream &s)
{
  s << BLT << src1 << " " << src2 << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_blti(char *src1, int imm, int label, ostream &s)
{
  s << BLT << src1 << " " << imm << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_bgti(char *src1, int imm, int label, ostream &s)
{
  s << BGT << src1 << " " << imm << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_branch(int l, ostream& s)
{
  s << BRANCH;
  emit_label_ref(l,s);
  s << endl;
}

//
// Push a register on the stack. The stack grows towards smaller addresses.
//
static void emit_push(char *reg, ostream& str)
{
  emit_store(reg,0,SP,str);
  emit_addiu(SP,SP,-4,str);
}

//
// Fetch the integer value in an Int object.
// Emits code to fetch the integer value of the Integer object pointed
// to by register source into the register dest
//
static void emit_fetch_int(char *dest, char *source, ostream& s)
{ emit_load(dest, DEFAULT_OBJFIELDS, source, s); }

//
// Emits code to store the integer value contained in register source
// into the Integer object pointed to by dest.
//
static void emit_store_int(char *source, char *dest, ostream& s)
{ emit_store(source, DEFAULT_OBJFIELDS, dest, s); }


static void emit_test_collector(ostream &s)
{
  emit_push(ACC, s);
  emit_move(ACC, SP, s); // stack end
  emit_move(A1, ZERO, s); // allocate nothing
  s << JAL << gc_collect_names[cgen_Memmgr] << endl;
  emit_addiu(SP,SP,4,s);
  emit_load(ACC,0,SP,s);
}

static void emit_gc_check(char *source, ostream &s)
{
  if (source != (char*)A1) emit_move(A1, source, s);
  s << JAL << "_gc_check" << endl;
}


///////////////////////////////////////////////////////////////////////////////
//
// coding strings, ints, and booleans
//
// Cool has three kinds of constants: strings, ints, and booleans.
// This section defines code generation for each type.
//
// All string constants are listed in the global "stringtable" and have
// type StringEntry.  StringEntry methods are defined both for String
// constant definitions and references.
//
// All integer constants are listed in the global "inttable" and have
// type IntEntry.  IntEntry methods are defined for Int
// constant definitions and references.
//
// Since there are only two Bool values, there is no need for a table.
// The two booleans are represented by instances of the class BoolConst,
// which defines the definition and reference methods for Bools.
//
///////////////////////////////////////////////////////////////////////////////

//
// Strings
//
void StringEntry::code_ref(ostream& s)
{
  s << STRCONST_PREFIX << index;
}

//
// Emit code for a constant String.
// You should fill in the code naming the dispatch table.
//

void StringEntry::code_def(ostream& s, int stringclasstag)
{
  IntEntryP lensym = inttable.add_int(len);

  // Add -1 eye catcher
  s << WORD << "-1" << endl;

  code_ref(s);  s  << LABEL                                             // label
      << WORD << stringclasstag << endl                                 // tag
      << WORD << (DEFAULT_OBJFIELDS + STRING_SLOTS + (len+4)/4) << endl // size
      << WORD; emit_disptable_ref(Str, s); s << endl;                 // dispatch table
      s << WORD;  lensym->code_ref(s);  s << endl;            // string length
  emit_string_constant(s,str);                                // ascii string
  s << ALIGN;                                                 // align to word
}

//
// StrTable::code_string
// Generate a string object definition for every string constant in the 
// stringtable.
//
void StrTable::code_string_table(ostream& s, int stringclasstag)
{  
  for (List<StringEntry> *l = tbl; l; l = l->tl())
    l->hd()->code_def(s,stringclasstag);
}

//
// Ints
//
void IntEntry::code_ref(ostream &s)
{
  s << INTCONST_PREFIX << index;
}

//
// Emit code for a constant Integer.
// You should fill in the code naming the dispatch table.
//

void IntEntry::code_def(ostream &s, int intclasstag)
{
  // Add -1 eye catcher
  s << WORD << "-1" << endl;

  code_ref(s);  s << LABEL                                // label
      << WORD << intclasstag << endl                      // class tag
      << WORD << (DEFAULT_OBJFIELDS + INT_SLOTS) << endl  // object size
      << WORD; emit_disptable_ref(Int, s); s << endl;     // dispatch table
      s << WORD << str << endl;                           // integer value
}


//
// IntTable::code_string_table
// Generate an Int object definition for every Int constant in the
// inttable.
//
void IntTable::code_string_table(ostream &s, int intclasstag)
{
  for (List<IntEntry> *l = tbl; l; l = l->tl())
    l->hd()->code_def(s,intclasstag);
}


//
// Bools
//
BoolConst::BoolConst(int i) : val(i) { assert(i == 0 || i == 1); }

void BoolConst::code_ref(ostream& s) const
{
  s << BOOLCONST_PREFIX << val;
}
  
//
// Emit code for a constant Bool.
// You should fill in the code naming the dispatch table.
//

void BoolConst::code_def(ostream& s, int boolclasstag)
{
  // Add -1 eye catcher
  s << WORD << "-1" << endl;

  code_ref(s);  s << LABEL                                  // label
      << WORD << boolclasstag << endl                       // class tag
      << WORD << (DEFAULT_OBJFIELDS + BOOL_SLOTS) << endl   // object size
      << WORD; emit_disptable_ref(Bool, s); s << endl;      // dispatch table
      s << WORD << val << endl;                             // value (0 or 1)
}

//////////////////////////////////////////////////////////////////////////////
//
//  CgenClassTable methods
//
//////////////////////////////////////////////////////////////////////////////

//***************************************************
//
//  Emit code to start the .data segment and to
//  declare the global names.
//
//***************************************************

void CgenClassTable::code_global_data()
{
  Symbol main    = idtable.lookup_string(MAINNAME);
  Symbol string  = idtable.lookup_string(STRINGNAME);
  Symbol integer = idtable.lookup_string(INTNAME);
  Symbol boolc   = idtable.lookup_string(BOOLNAME);

  str << "\t.data\n" << ALIGN;
  //
  // The following global names must be defined first.
  //
  str << GLOBAL << CLASSNAMETAB << endl;
  str << GLOBAL; emit_protobj_ref(main,str);    str << endl;
  str << GLOBAL; emit_protobj_ref(integer,str); str << endl;
  str << GLOBAL; emit_protobj_ref(string,str);  str << endl;
  str << GLOBAL; falsebool.code_ref(str);  str << endl;
  str << GLOBAL; truebool.code_ref(str);   str << endl;
  str << GLOBAL << INTTAG << endl;
  str << GLOBAL << BOOLTAG << endl;
  str << GLOBAL << STRINGTAG << endl;

  //
  // We also need to know the tag of the Int, String, and Bool classes
  // during code generation.
  //
  str << INTTAG << LABEL
      << WORD << intclasstag << endl;
  str << BOOLTAG << LABEL 
      << WORD << boolclasstag << endl;
  str << STRINGTAG << LABEL 
      << WORD << stringclasstag << endl;    
}


//***************************************************
//
//  Emit code to start the .text segment and to
//  declare the global names.
//
//***************************************************

void CgenClassTable::code_global_text()
{
  str << GLOBAL << HEAP_START << endl
      << HEAP_START << LABEL 
      << WORD << 0 << endl
      << "\t.text" << endl
      << GLOBAL;
  emit_init_ref(idtable.add_string("Main"), str);
  str << endl << GLOBAL;
  emit_init_ref(idtable.add_string("Int"),str);
  str << endl << GLOBAL;
  emit_init_ref(idtable.add_string("String"),str);
  str << endl << GLOBAL;
  emit_init_ref(idtable.add_string("Bool"),str);
  str << endl << GLOBAL;
  emit_method_ref(idtable.add_string("Main"), idtable.add_string("main"), str);
  str << endl;
}

void CgenClassTable::code_bools(int boolclasstag)
{
  falsebool.code_def(str,boolclasstag);
  truebool.code_def(str,boolclasstag);
}

void CgenClassTable::code_select_gc()
{
  //
  // Generate GC choice constants (pointers to GC functions)
  //
  str << GLOBAL << "_MemMgr_INITIALIZER" << endl;
  str << "_MemMgr_INITIALIZER:" << endl;
  str << WORD << gc_init_names[cgen_Memmgr] << endl;
  str << GLOBAL << "_MemMgr_COLLECTOR" << endl;
  str << "_MemMgr_COLLECTOR:" << endl;
  str << WORD << gc_collect_names[cgen_Memmgr] << endl;
  str << GLOBAL << "_MemMgr_TEST" << endl;
  str << "_MemMgr_TEST:" << endl;
  str << WORD << (cgen_Memmgr_Test == GC_TEST) << endl;
}
///////////////////////////////////////////////////////////////////////
//
// CgenTable code methods
//
///////////////////////////////////////////////////////////////////////

//********************************************************
//
// Emit code to reserve space for and initialize all of
// the constants.  Class names should have been added to
// the string table (in the supplied code, is is done
// during the construction of the inheritance graph), and
// code for emitting string constants as a side effect adds
// the string's length to the integer table.  The constants
// are emmitted by running through the stringtable and inttable
// and producing code for each entry.
//
//********************************************************

void CgenClassTable::code_constants()
{
  //
  // Add constants that are required by the code generator.
  //
  stringtable.add_string("");
  inttable.add_string("0");

  stringtable.code_string_table(str,stringclasstag);
  inttable.code_string_table(str,intclasstag);
  code_bools(boolclasstag);
}
//
// Set the classtags and code the class_nameTab
//
void CgenClassTable::code_class_nametab() {
  str << CLASSNAMETAB << LABEL;
  CgenNodeP curr_node = root();
  curr_node->build_class_tag(str, 0);
}

int CgenNode::build_class_tag(ostream& s, int index) {
  int temp_index = index;
  set_class_tag(temp_index++);
  s << WORD; stringtable.lookup_string(name->get_string())->code_ref(s); s << endl;
  for (List<CgenNode> *l = children; l; l = l->tl())
    temp_index = l->hd()->build_class_tag(s, temp_index);
  return temp_index;
}
//
// Code the dispatch tables
//
void CgenClassTable::build_methodtab() {
  for (List<CgenNode> *l = nds; l; l = l->tl()) {
    emit_disptable_ref(l->hd()->get_name(), str); str << LABEL;
    l->hd()->code_method_tab(str);
  }
}

void CgenNode::code_method_tab(ostream& s) {
  for (List<Entry> *l = method_list; l; l = l->tl()) {
    Symbol curr_name = l->hd();
    s << WORD; code_method_name(s, curr_name); s << endl;
  }
}

void CgenNode::code_method_name(ostream& s, Symbol method_name) {
  s << method_symbtab->lookup(method_name) << "." << method_name;
}
//
// Code the proto-objects
//
void CgenClassTable::code_protoobj() {
  for (List<CgenNode> *l = nds; l; l = l->tl()) {
    if (l->hd()->get_name() == Str)
      code_string_protobj();
    else
      l->hd()->code_class_protobj(str);
  }
}

void CgenNode::code_class_protobj(ostream &s) {
  int attr_num = get_attr_num();
  s << WORD << "-1" << endl;
  emit_protobj_ref(name,s);  s << LABEL         // label
      << WORD << class_tag << endl                       // class tag
      << WORD << (DEFAULT_OBJFIELDS + attr_num) << endl //object size
      << WORD; emit_disptable_ref(name, s); s << endl; // dispatch table
  Symbol curr_type;
  for (List<Entry> *l = attr_list; l; l = l->tl()) 
    s << WORD << 0 << endl;
}

void CgenClassTable::code_string_protobj() {
  str << WORD << "-1" << endl;
  emit_protobj_ref(idtable.lookup_string(STRINGNAME),str);  str << LABEL // label
      << WORD << stringclasstag << endl                         // tag
      << WORD << (DEFAULT_OBJFIELDS + STRING_SLOTS + 1) << endl // size
      << WORD; emit_disptable_ref(Str, str); str << endl;       // dispatch table
      str << WORD << INTCONST_PREFIX << 0 << endl;              // string length
  emit_string_constant(str,"");                                 // ascii string
  str << ALIGN;                                                 // align to word
}
//
// Code initialization methods
//
void CgenClassTable::code_init() {
  for (List<CgenNode> *l = nds; l; l = l->tl())
    l->hd()->code_init(str, this, label_index);
}

void CgenNode::code_init(ostream &s, CgenClassTableP curr_classtable, int &label_index) {
  s << name << CLASSINIT_SUFFIX << LABEL;
  // Save all the callee-saved registers
  emit_addiu(SP, SP, -16, s);
  emit_store(FP, 4, SP, s);
  emit_store(SELF, 3, SP, s);
  emit_store("$s1", 2, SP, s);
  emit_store(RA, 1, SP, s);
  emit_addiu(FP, SP, 4, s);
  emit_move(SELF, ACC, s);
  if (name != Object) {
    // Call parent's init method
    s << JAL << parent << CLASSINIT_SUFFIX << endl;
    int curr_attr_num = parentnd->get_attr_num()+3;
    for (int i = features->first(); features->more(i); i = features->next(i)) {
      Feature curr_feature = features->nth(i);
      if (curr_feature->attr_flag())
        curr_feature->code(s, this, curr_classtable, label_index);
      emit_store(ACC, curr_attr_num++, SELF, s);
    }
  }
  emit_move(ACC, SELF, s);
  emit_load(FP, 4, SP, s);
  emit_load(SELF, 3, SP, s);
  emit_load("$s1", 2, SP, s);
  emit_load(RA, 1, SP, s);
  emit_addiu(SP, SP, 16, s);
  emit_return(s);
}
//
// Code the class_objTab
//
void CgenClassTable::code_objtab() {
  str << CLASSOBJTAB << LABEL;
  root()->code_objtab(str);
}

void CgenNode::code_objtab(ostream &s) {
  s << WORD << name << PROTOBJ_SUFFIX << endl;
  s << WORD << name << CLASSINIT_SUFFIX << endl;
  // In the order of class tag
  for (List<CgenNode> *l = children; l; l = l->tl())
    l->hd()->code_objtab(s);
}
//
// Code the methods
//
void CgenClassTable::code_method() {
  for (List<CgenNode> *l = nds; l; l = l->tl()) {
    if (!l->hd()->basic())
      l->hd()->code_method(str, this, label_index);
  }
}

void CgenNode::code_method(ostream &s, CgenClassTableP curr_classtable, int &label_index) {
  for (int i = features->first(); features->more(i); i = features->next(i)){
    features->nth(i)->code_method(s, this, curr_classtable, label_index);
  }
}

void method_class::code_method(ostream& s, CgenNodeP curr_node, CgenClassTableP curr_classtable, int& label_index) {
  s << curr_node->get_name() << "." << name << LABEL;
  // Save all the callee-saved registers
  emit_addiu(SP, SP, -16, s);
  emit_store(FP, 4, SP, s);
  emit_store(SELF, 3, SP, s);
  emit_store("$s1", 2, SP, s);
  emit_store(RA, 1, SP, s);
  emit_addiu(FP, SP, 4, s);
  emit_move(SELF, ACC, s);
  for (int i = formals->first(); formals->more(i); i = formals->next(i))
    curr_node->add_formal_id(formals->nth(i)->get_name());
  expr->code(s, curr_node, curr_classtable, label_index);
  emit_load(FP, 4, SP, s);
  emit_load(SELF, 3, SP, s);
  emit_load("$s1", 2, SP, s);
  emit_load(RA, 1, SP, s);
  emit_addiu(SP, SP, 16, s);
  emit_return(s);
  curr_node->empty_formal_id();
}

///////////////////////////////////////////////////////////////////////
//
// Build the Inheritance Graph
//
///////////////////////////////////////////////////////////////////////

CgenClassTable::CgenClassTable(Classes classes, ostream& s) : nds(NULL) , str(s), class_num(0), label_index(0)
{
   stringclasstag = 4 /* Change to your String class tag here */;
   intclasstag =    2 /* Change to your Int class tag here */;
   boolclasstag =   3 /* Change to your Bool class tag here */;

   enterscope();
   if (cgen_debug) cout << "Building CgenClassTable" << endl;
   install_basic_classes();
   install_classes(classes);
   build_inheritance_tree();
   build_feature_tab();

   code();
   exitscope();
}

void CgenClassTable::install_basic_classes()
{

// The tree package uses these globals to annotate the classes built below.
  //curr_lineno  = 0;
  Symbol filename = stringtable.add_string("<basic class>");

//
// A few special class names are installed in the lookup table but not
// the class list.  Thus, these classes exist, but are not part of the
// inheritance hierarchy.
// No_class serves as the parent of Object and the other special classes.
// SELF_TYPE is the self class; it cannot be redefined or inherited.
// prim_slot is a class known to the code generator.
//
  addid(No_class,
	new CgenNode(class_(No_class,No_class,nil_Features(),filename),
			    Basic,this));
  addid(SELF_TYPE,
	new CgenNode(class_(SELF_TYPE,No_class,nil_Features(),filename),
			    Basic,this));
  addid(prim_slot,
	new CgenNode(class_(prim_slot,No_class,nil_Features(),filename),
			    Basic,this));

// 
// The Object class has no parent class. Its methods are
//        cool_abort() : Object    aborts the program
//        type_name() : Str        returns a string representation of class name
//        copy() : SELF_TYPE       returns a copy of the object
//
// There is no need for method bodies in the basic classes---these
// are already built in to the runtime system.
//
  install_class(
   new CgenNode(
    class_(Object, 
	   No_class,
	   append_Features(
           append_Features(
           single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
           single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
           single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
	   filename),
    Basic,this));

// 
// The IO class inherits from Object. Its methods are
//        out_string(Str) : SELF_TYPE          writes a string to the output
//        out_int(Int) : SELF_TYPE               "    an int    "  "     "
//        in_string() : Str                    reads a string from the input
//        in_int() : Int                         "   an int     "  "     "
//
   install_class(
    new CgenNode(
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
	   filename),	    
    Basic,this));

//
// The Int class has no methods and only a single attribute, the
// "val" for the integer. 
//
   install_class(
    new CgenNode(
     class_(Int, 
	    Object,
            single_Features(attr(val, prim_slot, no_expr())),
	    filename),
     Basic,this));

//
// Bool also has only the "val" slot.
//
    install_class(
     new CgenNode(
      class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename),
      Basic,this));

//
// The class Str has a number of slots and operations:
//       val                                  ???
//       str_field                            the string itself
//       length() : Int                       length of the string
//       concat(arg: Str) : Str               string concatenation
//       substr(arg: Int, arg2: Int): Str     substring
//       
   install_class(
    new CgenNode(
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
	     filename),
        Basic,this));

}

// CgenClassTable::install_class
// CgenClassTable::install_classes
//
// install_classes enters a list of classes in the symbol table.
//

void CgenClassTable::install_class(CgenNodeP nd)
{
  Symbol name = nd->get_name();

  if (probe(name))
    {
      return;
    }

  // The class name is legal, so add it to the list of classes
  // and the symbol table.
  nds = list_enqueue(nds, nd); // Changed to enqueue
  addid(name,nd);
  class_num++;
}

void CgenClassTable::install_classes(Classes cs)
{
  for(int i = cs->first(); cs->more(i); i = cs->next(i))
    install_class(new CgenNode(cs->nth(i),NotBasic,this));
}

//
// CgenClassTable::build_inheritance_tree
//
void CgenClassTable::build_inheritance_tree()
{
  for(List<CgenNode> *l = nds; l; l = l->tl())
      set_relations(l->hd());
}

//
// CgenClassTable::set_relations
//
// Takes a CgenNode and locates its, and its parent's, inheritance nodes
// via the class table.  Parent and child pointers are added as appropriate.
//
void CgenClassTable::set_relations(CgenNodeP nd)
{
  CgenNode *parent_node = probe(nd->get_parent());
  nd->set_parentnd(parent_node);
  parent_node->add_child(nd);
}

void CgenNode::add_child(CgenNodeP n)
{
  children = list_enqueue(children, n); // Changed to enqueue
}

void CgenNode::set_parentnd(CgenNodeP p)
{
  assert(parentnd == NULL);
  assert(p != NULL);
  parentnd = p;
}
//
// Record all the features of a certain class
//
void CgenClassTable::build_feature_tab() {
  for(List<CgenNode> *l = nds; l; l = l->tl())
    l->hd()->build_feature_tab();
}

void CgenNode::build_feature_tab() {
  attr_list = build_attr_tab_acc(attr_list, 1);
  method_list = build_method_tab_acc(method_list, method_symbtab, 0);
}

List<Entry> *CgenNode::build_attr_tab_acc(List<Entry> *feature_list, int flag) {
  if (name != Object) 
    return add_attr(parentnd->build_attr_tab_acc(feature_list, flag), flag);
  return add_attr(feature_list, flag);
}

List<Entry> *CgenNode::build_method_tab_acc(List<Entry> *feature_list, SymbolTable<Symbol, Entry> *feature_symbtab, int flag) {
  if (name != Object) 
    return add_method(parentnd->build_method_tab_acc(feature_list, feature_symbtab, flag), feature_symbtab, flag);
  return add_method(feature_list, feature_symbtab, flag);
}

List<Entry> *CgenNode::add_attr(List<Entry> *feature_list, int flag) {
  for (int i = features->first(); features->more(i); i = features->next(i)) {
    Feature curr_feature = features->nth(i);
    if (curr_feature->attr_flag() == flag)
      feature_list = add_attr_acc(feature_list, curr_feature);
  }
  return feature_list;
}

List<Entry> *CgenNode::add_method(List<Entry> *feature_list, SymbolTable<Symbol, Entry> *feature_symbtab, int flag) {
  for (int i = features->first(); features->more(i); i = features->next(i)) {
    Feature curr_feature = features->nth(i);
    if (curr_feature->attr_flag() == flag)
      feature_list = add_method_acc(feature_list, feature_symbtab, curr_feature);
  }
  return feature_list;
}

List<Entry> *CgenNode::add_attr_acc(List<Entry> *feature_list, Feature curr_feature) {
  Symbol curr_name = curr_feature->get_name();
  feature_list = list_enqueue(feature_list, curr_name);
  return feature_list;
}

List<Entry> *CgenNode::add_method_acc(List<Entry> *feature_list, SymbolTable<Symbol, Entry> *feature_symbtab, Feature curr_feature) {
  Symbol curr_name = curr_feature->get_name();
  feature_symbtab->addid(curr_name, name);
  feature_list = list_enqueue(feature_list, curr_name);
  return feature_list;
}

void CgenClassTable::code()
{
  if (cgen_debug) cout << "coding global data" << endl;
  code_global_data();

  if (cgen_debug) cout << "choosing gc" << endl;
  code_select_gc();

  if (cgen_debug) cout << "coding constants" << endl;
  code_constants();

  if (cgen_debug) cout << "coding class_nameTab" << endl;
  code_class_nametab();
  code_protoobj();
  build_methodtab();
  code_objtab();
  
  if (cgen_debug) cout << "coding global text" << endl;
  code_global_text();
  code_init();
  code_method();
}


CgenNodeP CgenClassTable::root()
{
   return probe(Object);
}

///////////////////////////////////////////////////////////////////////
//
// CgenTable helper methods
//
///////////////////////////////////////////////////////////////////////

List<CgenNode> *CgenClassTable::list_enqueue(List<CgenNode> *l, CgenNodeP nd) {
  if (l == NULL)
    return new List<CgenNode>(nd, l);
  else
    return new List<CgenNode>(l->hd(), list_enqueue(l->tl(), nd));
}

int CgenClassTable::get_method_offset(Symbol class_name, Symbol method_name) {
  return lookup(class_name)->get_method_offset(method_name);  
}

List<Case_class> *CgenClassTable::sort(List<Case_class> *node_list) {
  if (node_list->tl() == NULL)
    return node_list;
  Case max_node;
  Case curr_node;
  int max_index = 0;
  int curr_index;
  for (List<Case_class> *l = node_list; l; l = l->tl()) {
    curr_node = l->hd();
    curr_index = lookup(l->hd()->get_type())->get_class_tag();
    if (curr_index > max_index) {
      max_node = curr_node;
      max_index = curr_index;
    }
  }
  return new List<Case_class>(max_node, sort(remove_entry(node_list, max_node)));
}

List<Case_class> *CgenClassTable::remove_entry(List<Case_class> *node_list, Case node) {
  if (node_list->hd() == node)
    return node_list->tl();
  return new List<Case_class>(node_list->hd(), remove_entry(node_list->tl(), node));
}

void CgenClassTable::code_typcase(Symbol node, int label) {
  CgenNodeP curr_node = lookup(node);
  emit_blti(T1, curr_node->get_class_tag(), label, str);
  emit_bgti(T1, curr_node->find_max_child(), label, str);
}

///////////////////////////////////////////////////////////////////////
//
// CgenNode helper methods
//
///////////////////////////////////////////////////////////////////////

CgenNode::CgenNode(Class_ nd, Basicness bstatus, CgenClassTableP ct) :
   class__class((const class__class &) *nd),
   parentnd(NULL),
   children(NULL),
   basic_status(bstatus),
   attr_list(NULL),
   method_list(NULL),
   let_list(NULL),
   formal_list(NULL)
{ 
   method_symbtab = new SymbolTable<Symbol, Entry>();
   method_symbtab->enterscope();
   stringtable.add_string(name->get_string());          // Add class name to string table
}

List<CgenNode> *CgenNode::list_enqueue(List<CgenNode> *l, CgenNodeP nd) {
  if (l == NULL)
    return new List<CgenNode>(nd, l);
  else
    return new List<CgenNode>(l->hd(), list_enqueue(l->tl(), nd));
}

List<Entry> *CgenNode::list_enqueue(List<Entry> *l, Symbol s) {
  if (l == NULL)
    return new List<Entry>(s, l);
  else
    return new List<Entry>(l->hd(), list_enqueue(l->tl(), s));
}

int CgenNode::get_attr_offset(Symbol attr_name) {
  int v = 3;
  for (List<Entry> *l = attr_list; l; l = l->tl()) {
    if (l->hd() == attr_name)
      return v;
    else
      v++;
  }
  return v;
}

int CgenNode::get_method_offset(Symbol method_name) {
  int curr_index = 0;
  for (List<Entry> *l = method_list; l; l = l->tl()) {
    if (l->hd() == method_name)
      return curr_index;
    curr_index++;
  }
  return curr_index;
}

int CgenNode::get_let_offset(Symbol name) {
  int v = list_length(let_list);
  for (List<Entry> *l = let_list; l; l = l->tl()) {
    if (l->hd() == name)
      return v;
    else
      v--;
  }
  return -1;
}

int CgenNode::get_formal_offset(Symbol name) {
  int v = 0;
  for (List<Entry> *l = formal_list; l; l = l->tl()) {
    if (l->hd() == name)
      return v;
    else
      v++;
  }
  return -1;
}

void CgenNode::add_let_id(Symbol name) {
  let_list = new List<Entry>(name, let_list);
}

void CgenNode::remove_let_id() {
  let_list = let_list->tl();
}

void CgenNode::add_formal_id(Symbol name) {
  formal_list = new List<Entry>(name, formal_list);
}

void CgenNode::empty_formal_id() {
  formal_list = NULL;
}
//
// Find the largest class tag among its children. Used in coding cases.
//
int CgenNode::find_max_child() {
  if (children == NULL)
    return class_tag;
  for (List<CgenNode> *l = children; l; l = l->tl()) {
    if (l->tl() == NULL)
      return l->hd()->find_max_child();
  }
}



//******************************************************************
//
//   Fill in the following methods to produce code for the
//   appropriate expression.  You may add or remove parameters
//   as you wish, but if you do, remember to change the parameters
//   of the declarations in `cool-tree.h'  Sample code for
//   constant integers, strings, and booleans are provided.
//
//*****************************************************************

void assign_class::code(ostream &s, CgenNodeP curr_node, CgenClassTableP curr_classtable, int &label_index) {
  expr->code(s, curr_node, curr_classtable, label_index);
  int offset = curr_node->get_let_offset(name);
  if (offset != -1)
    emit_store(ACC, -offset, FP, s);
  else {
    offset = curr_node->get_formal_offset(name);
    if (offset != -1)
      emit_store(ACC, offset+3, FP, s);
    else {
      offset = curr_node->get_attr_offset(name);
      emit_store(ACC, offset, SELF, s);
    }
  }
  if (cgen_Memmgr) {
    emit_addiu(A1, SELF, curr_node->get_attr_offset(name), s);
    emit_gc_assign(s);
  }
}

void static_dispatch_class::code(ostream &s, CgenNodeP curr_node, CgenClassTableP curr_classtable, int &label_index) {
  for (int i = actual->first(); actual->more(i); i = actual->next(i)) {
    actual->nth(i)->code(s, curr_node, curr_classtable, label_index);
    emit_push(ACC, s);
  }
  expr->code(s, curr_node, curr_classtable, label_index);
  emit_bne(ACC, ZERO, ++label_index, s);
  emit_load_imm(T1, get_line_number(), s);
  s << JAL << "_dispatch_abort" << endl;
  emit_label_def(label_index, s);
  s << JAL << type_name << "." << name << endl;
}

void dispatch_class::code(ostream &s, CgenNodeP curr_node, CgenClassTableP curr_classtable, int &label_index) {
  for (int i = actual->first(); actual->more(i); i = actual->next(i)) {
    actual->nth(i)->code(s, curr_node, curr_classtable, label_index);
    emit_push(ACC, s);
  }
  expr->code(s, curr_node, curr_classtable, label_index);
  emit_bne(ACC, ZERO, ++label_index, s);
  emit_load_imm(T1, get_line_number(), s);
  s << JAL << "_dispatch_abort" << endl;
  emit_label_def(label_index, s);
  emit_load(T1, 2, ACC, s);
  int offset = curr_classtable->get_method_offset(expr->get_type(), name);
  emit_load(T1, offset, T1, s);
  emit_jalr(T1, s);
}

void cond_class::code(ostream &s, CgenNodeP curr_node, CgenClassTableP curr_classtable, int &label_index) {
  pred->code(s, curr_node, curr_classtable, label_index);
  emit_load(T1, 3, ACC, s);
  emit_bne(ACC, ZERO, ++label_index, s);
  int temp_label = label_index;
  else_exp->code(s, curr_node, curr_classtable, ++label_index);
  emit_branch(temp_label+1, s);
  emit_label_def(temp_label, s);
  then_exp->code(s, curr_node, curr_classtable, label_index);
  emit_label_def(temp_label+1, s);
}

void loop_class::code(ostream &s, CgenNodeP curr_node, CgenClassTableP curr_classtable, int &label_index) {
  int temp_label = label_index + 1;
  label_index = label_index + 3;
  emit_label_def(temp_label, s);
  pred->code(s, curr_node, curr_classtable, label_index);
  emit_load(T1, 3, ACC, s);
  emit_bne(ACC, ZERO, temp_label+1, s);
  emit_branch(temp_label+2, s);
  emit_label_def(temp_label+1, s);
  body->code(s, curr_node, curr_classtable, label_index);
  emit_branch(temp_label, s);
  emit_label_def(temp_label+2, s);
  emit_load_imm(ACC, 0, s);
}

void typcase_class::code(ostream &s, CgenNodeP curr_node, CgenClassTableP curr_classtable, int &label_index) {
  expr->code(s, curr_node, curr_classtable, label_index);
  emit_load(T1, 0, ACC, s);
  emit_bne(ACC, ZERO, label_index+1, s);
  emit_partial_load_address(ACC, s); s << STRCONST_PREFIX << "0" << endl;
  emit_load_imm(T1, get_line_number(), s);
  s << JAL << "_case_abort2" << endl;
  List<Case_class> *branch_list = NULL;
  for (int i = cases->first(); cases->more(i); i = cases->next(i))
    branch_list = new List<Case_class>(cases->nth(i), branch_list);
  branch_list = curr_classtable->sort(branch_list);
  Case curr_branch;
  int curr_label = label_index+1;
  int final_label = label_index + list_length(branch_list) + 2;
  label_index = final_label;
  for (List<Case_class> *l = branch_list; l; l = l->tl()) {
    emit_label_def(curr_label, s);
    curr_branch = l->hd();
    curr_classtable->code_typcase(curr_branch->get_type(), ++curr_label);
    curr_node->add_let_id(curr_branch->get_name());
    emit_push(ACC, s);
    curr_branch->get_expr()->code(s, curr_node, curr_classtable, label_index);
    emit_branch(final_label, s);
  }
  emit_label_def(curr_label++, s);
  s << JAL << "_case_abort" << endl;
  emit_label_def(curr_label, s);
  emit_addiu(SP, SP, 4, s);
}

void block_class::code(ostream &s, CgenNodeP curr_node, CgenClassTableP curr_classtable, int &label_index) {
  for (int i = body->first(); body->more(i); i = body->next(i))
    body->nth(i)->code(s, curr_node, curr_classtable, label_index);
}

void let_class::code(ostream &s, CgenNodeP curr_node, CgenClassTableP curr_classtable, int &label_index) {
  curr_node->add_let_id(identifier);
  init->code(s, curr_node, curr_classtable, label_index);
  emit_push(ACC, s);
  body->code(s, curr_node, curr_classtable, label_index);
  emit_addiu(SP, SP, 4, s);
  curr_node->remove_let_id();
}

void plus_class::code(ostream &s, CgenNodeP curr_node, CgenClassTableP curr_classtable, int &label_index) {
  emit_push("$s1", s);
  e1->code(s, curr_node, curr_classtable, label_index);
  emit_fetch_int("$s1", ACC, s);
  e2->code(s, curr_node, curr_classtable, label_index);
  emit_fetch_int(ACC, ACC, s);
  emit_add("$s1", "$s1", ACC, s);
  emit_partial_load_address(ACC, s); emit_protobj_ref(Int, s); s << endl;
  emit_jal("Object.copy", s);
  emit_store_int("$s1", ACC, s);
  emit_load("$s1", 1, SP, s);
  emit_addiu(SP, SP, 4, s);
}

void sub_class::code(ostream &s, CgenNodeP curr_node, CgenClassTableP curr_classtable, int &label_index) {
  emit_push("$s1", s);
  e1->code(s, curr_node, curr_classtable, label_index);
  emit_fetch_int("$s1", ACC, s);
  e2->code(s, curr_node, curr_classtable, label_index);
  emit_fetch_int(ACC, ACC, s);
  emit_sub("$s1", "$s1", ACC, s);
  emit_partial_load_address(ACC, s); emit_protobj_ref(Int, s); s << endl;
  emit_jal("Object.copy", s);
  emit_store_int("$s1", ACC, s);
  emit_load("$s1", 1, SP, s);
  emit_addiu(SP, SP, 4, s);
}

void mul_class::code(ostream &s, CgenNodeP curr_node, CgenClassTableP curr_classtable, int &label_index) {
  emit_push("$s1", s);
  e1->code(s, curr_node, curr_classtable, label_index);
  emit_fetch_int("$s1", ACC, s);
  e2->code(s, curr_node, curr_classtable, label_index);
  emit_fetch_int(ACC, ACC, s);
  emit_mul("$s1", "$s1", ACC, s);
  emit_partial_load_address(ACC, s); emit_protobj_ref(Int, s); s << endl;
  emit_jal("Object.copy", s);
  emit_store_int("$s1", ACC, s);
  emit_load("$s1", 1, SP, s);
  emit_addiu(SP, SP, 4, s);
}

void divide_class::code(ostream &s, CgenNodeP curr_node, CgenClassTableP curr_classtable, int &label_index) {
  emit_push("$s1", s);
  e1->code(s, curr_node, curr_classtable, label_index);
  emit_fetch_int("$s1", ACC, s);
  e2->code(s, curr_node, curr_classtable, label_index);
  emit_fetch_int(ACC, ACC, s);
  emit_div("$s1", "$s1", ACC, s);
  emit_partial_load_address(ACC, s); emit_protobj_ref(Int, s); s << endl;
  emit_jal("Object.copy", s);
  emit_store_int("$s1", ACC, s);
  emit_load("$s1", 1, SP, s);
  emit_addiu(SP, SP, 4, s);
}

void neg_class::code(ostream &s, CgenNodeP curr_node, CgenClassTableP curr_classtable, int &label_index) {
  e1->code(s, curr_node, curr_classtable, label_index);
  emit_fetch_int(T1, ACC, s);
  emit_neg("$s1", T1, s);
  emit_partial_load_address(ACC, s); emit_protobj_ref(Int, s); s << endl;
  emit_jal("Object.copy", s);
  emit_store_int("$s1", ACC, s);
}

void lt_class::code(ostream &s, CgenNodeP curr_node, CgenClassTableP curr_classtable, int &label_index) {
  emit_push("$s1", s);
  e1->code(s, curr_node, curr_classtable, label_index);
  emit_fetch_int("$s1", ACC, s);
  e2->code(s, curr_node, curr_classtable, label_index);
  emit_fetch_int(ACC, ACC, s);
  emit_blt("$s1", ACC, ++label_index, s);
  emit_load_bool(ACC, BoolConst(0), s);
  emit_branch(label_index+1, s);
  emit_label_def(label_index, s);
  emit_load_bool(ACC, BoolConst(1), s);
  emit_branch(++label_index, s);
  emit_label_def(label_index, s);
  emit_load("$s1", 1, SP, s);
  emit_addiu(SP, SP, 4, s);
}

void eq_class::code(ostream &s, CgenNodeP curr_node, CgenClassTableP curr_classtable, int &label_index) {
  emit_push("$s1", s);
  e1->code(s, curr_node, curr_classtable, label_index);
  emit_move("$s1", ACC, s);
  e2->code(s, curr_node, curr_classtable, label_index);
  emit_move(T2, ACC, s);
  emit_move(T1, "$s1", s);
  emit_load_bool(ACC, BoolConst(1), s);
  emit_load_bool(A1, BoolConst(0), s);
  emit_jal("equality_test", s);
  emit_load("$s1", 1, SP, s);
  emit_addiu(SP, SP, 4, s);
}

void leq_class::code(ostream &s, CgenNodeP curr_node, CgenClassTableP curr_classtable, int &label_index) {
  emit_push("$s1", s);
  e1->code(s, curr_node, curr_classtable, label_index);
  emit_fetch_int("$s1", ACC, s);
  e2->code(s, curr_node, curr_classtable, label_index);
  emit_fetch_int(ACC, ACC, s);
  emit_bleq("$s1", ACC, ++label_index, s);
  emit_load_bool(ACC, BoolConst(0), s);
  emit_branch(label_index+1, s);
  emit_label_def(label_index, s);
  emit_load_bool(ACC, BoolConst(1), s);
  emit_branch(++label_index, s);
  emit_label_def(label_index, s);
  emit_load("$s1", 1, SP, s);
  emit_addiu(SP, SP, 4, s);
}

void comp_class::code(ostream &s, CgenNodeP curr_node, CgenClassTableP curr_classtable, int &label_index) {
  e1->code(s, curr_node, curr_classtable, label_index);
  emit_move(T1, ACC, s);
  emit_load_bool(T2, BoolConst(0), s);
  emit_load_bool(ACC, BoolConst(1), s);
  emit_load_bool(A1, BoolConst(0), s);
  emit_jal("equality_test", s);
}

void int_const_class::code(ostream& s, CgenNodeP curr_node, CgenClassTableP curr_classtable, int &label_index)  
{
  //
  // Need to be sure we have an IntEntry *, not an arbitrary Symbol
  //
  emit_load_int(ACC,inttable.lookup_string(token->get_string()),s);
}

void string_const_class::code(ostream& s, CgenNodeP curr_node, CgenClassTableP curr_classtable, int &label_index)
{
  emit_load_string(ACC,stringtable.lookup_string(token->get_string()),s);
}

void bool_const_class::code(ostream& s, CgenNodeP curr_node, CgenClassTableP curr_classtable, int &label_index)
{
  emit_load_bool(ACC, BoolConst(val), s);
}

void new__class::code(ostream &s, CgenNodeP curr_node, CgenClassTableP curr_classtable, int &label_index) {
  if (type_name == SELF_TYPE) {
    emit_partial_load_address(T1, s); s << CLASSOBJTAB << endl;
    emit_load(T2, 0, SELF, s);
    emit_sll(T2, T2, 3, s);
    emit_addu(T1, T1, T2, s);
    emit_move("$s1", T1, s);
    emit_load(ACC, 0, T3, s);
    emit_jal("Object.copy", s);
    emit_load(T1, 1, "$s1", s);
    emit_jalr(T1, s);
  }
  else{
    emit_partial_load_address(ACC, s); s << type_name << PROTOBJ_SUFFIX << endl;
    emit_jal("Object.copy", s);
    s << JAL << type_name << CLASSINIT_SUFFIX << endl;
  }
}

void isvoid_class::code(ostream &s, CgenNodeP curr_node, CgenClassTableP curr_classtable, int &label_index) {
  e1->code(s, curr_node, curr_classtable, label_index);
  emit_beqz(ACC, ++label_index, s);
  emit_load_bool(ACC, BoolConst(0), s);
  emit_branch(label_index+1, s);
  emit_label_def(label_index, s);
  emit_load_bool(ACC, BoolConst(1), s);
  emit_branch(++label_index, s);
  emit_label_def(label_index, s);
}

void no_expr_class::code(ostream &s, CgenNodeP curr_node, CgenClassTableP curr_classtable, int &label_index) {
  if (type == Int) 
    emit_load_int(ACC, inttable.lookup_string("0"), s);
  else if (type == Str)
    emit_load_string(ACC, stringtable.lookup_string(""), s);
  else if (type == Bool)
    emit_load_bool(ACC, BoolConst(0), s);
  else
    emit_load_imm(ACC, 0, s);
}

void object_class::code(ostream &s, CgenNodeP curr_node, CgenClassTableP curr_classtable, int &label_index) {
  if (name == self)
    emit_move(ACC, SELF, s);
  else {
    int offset = curr_node->get_let_offset(name);
    if (offset != -1)
      emit_load(ACC, -offset, FP, s);
    else {
      offset = curr_node->get_formal_offset(name);
      if (offset != -1)
        emit_load(ACC, offset+3, FP, s);
      else {
        offset = curr_node->get_attr_offset(name);
        emit_load(ACC, offset, SELF, s);
      }
    }
  }
}


