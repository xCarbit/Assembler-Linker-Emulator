#include <string>
#include <regex>
#include <iostream>

using namespace std;


string sc_LiteralDecimal="-?[0-9]+";
string sc_LiteralHexa= "0x[0-9A-F]+";
string sc_Symbol="[a-zA-Z](?:[a-zA-Z0-9_]|\\s)*";
string sc_register="[0-15]";
string sc_operation="\\+";

//Regex for directives
regex emptyLine("^\\s*$");
regex comment("^#\\s*.*$");
regex digit("0|1|2|4|4|5|6|7|8|9");
regex symbolDirective("([a-zA-Z][a-zA-Z0-9_]*):");
regex globalDirective("^\\.global (" + sc_Symbol + "(,\\s*" + sc_Symbol + ")*)$");
regex externDirective("^\\.extern (" + sc_Symbol + "(,\\s*" + sc_Symbol + ")*)$");
regex wordDirective("^\\.word ((?:" + sc_LiteralDecimal + "|" + sc_Symbol +"|" + sc_LiteralHexa+  ")(,\\s*(?:" + sc_LiteralDecimal + "|" + sc_Symbol +"|"+ sc_LiteralHexa+  "))*)$");
regex sectionDirective("^\\.section\\s+("+sc_Symbol+")$");
regex skipDirective("^\\.skip\\s+("+sc_LiteralHexa+"|" + sc_LiteralDecimal +")$");
regex endDirective("^\\.end$");
regex asciiDirective("^\\.ascii\\s+\"("+sc_Symbol+")\"$");


regex equDirective("^\\.equ\\s+([a-zA-Z][a-zA-Z0-9_]*),\\s*((?:[a-zA-Z][a-zA-Z0-9_]*)|(?:\\s)|(?:-?[0-9]+)|(?:0x[0-9A-F]+)|(?:\\+)|(?:-))+$");

//Regex for st instruction

regex instStMemDir("^\\s*st\\s+(%r(?:0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15)|%pc|%sp), (-?[0-9]+|0x[0-9A-F]+|[a-zA-Z][a-zA-Z0-9_]*)\\s*(#.*)?$");
regex instStRegDir("^\\s*st\\s+(%r(?:0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15)|%pc|%sp),\\s*(%r(?:0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15)|%pc|%sp)\\s*(#.*)?$");
regex instStRegInd("^\\s*st\\s+(%r(?:0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15)|%pc|%sp),\\s+\\[(%r(?:0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15)|%pc|%sp)\\]\\s*(#.*)?$");
regex instStRegIndPomLit("^\\s*st\\s+(%r(?:0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15)|%pc|%sp),\\s*\\[(%r(?:0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15)|%pc|%sp)\\s*\\+\\s*(-?[0-9]+|0x[0-9A-F]+)\\]\\s*(#.*)?$");
regex instStRegIndPomSym("^\\s*st\\s+(%r(?:0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15)|%pc|%sp),\\s*\\[(%r(?:0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15)|%pc|%sp)\\s*\\+\\s*([a-zA-Z][a-zA-Z0-9_]*)\\]\\s*(#.*)?$");
//Regex for instructions

regex instrWithoutOperand("^\\s*(halt|int|iret|ret)\\s*(#.*)?$");
regex instWithOneOperand("^\\s*(push|pop|not|call|jmp)\\s+(%r(?:0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15)|%pc|%sp|-?[0-9]+|0x[0-9A-F]+|[a-zA-Z][a-zA-Z0-9_]*)\\s*(#.*)?$");
regex instBranches("^\\s*(beq|bne|bgt)\\s+(%r(?:0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15)|%pc|%sp),\\s*(%r(?:0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15)|%pc|%sp),\\s*([0-9]+|0x[0-9A-F]+|[a-zA-Z][a-zA-Z0-9_]*)\\s*(#.*)?$");
regex instAritmLogic("^\\s*(xchg|add|sub|mul|div|and|or|xor|shl|shr)\\s+(%r(?:0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15)|%pc|%sp),\\s*(%r(?:0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15)|%pc|%sp)\\s*(#.*)?$");
regex instrCSRRD("^\\s*csrrd\\s+%(status|handler|cause),\\s*(%r(?:0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15)|%pc|%sp)\\s*(#.*)?$");
regex instrCSRWR("^\\s*csrwr\\s+(%r(?:0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15)|%pc|%sp),\\s*%(status|handler|cause)\\s*(#.*)?$");

//Regex for ld instruction

regex instrLdImm("^\\s*ld\\s+\\$(-?[0-9]+|0x[0-9A-F]+|[a-zA-Z][a-zA-Z0-9_]*),\\s*(%r(?:0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15)|%pc|%sp)\\s*(#.*)?$");
regex instrLdRegDir("^\\s*ld\\s+(%r(?:0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15)|%pc|%sp),\\s*(%r(?:0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15)|%pc|%sp)\\s*(#.*)?$");
regex instrLdMemDir("^\\s*ld\\s+(-?[0-9]+|0x[0-9A-F]+|[a-zA-Z][a-zA-Z0-9_]*),\\s*(%r(?:0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15)|%pc|%sp)\\s*(#.*)?$");
regex instrLdRegInd("^\\s*ld\\s+\\[(%r(?:0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15)|%pc|%sp)\\],\\s*(%r(?:0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15)|%pc|%sp)\\s*(#.*)?$");
regex instrLdRegIndPomLit("^\\s*ld\\s+\\[(%r(?:0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15)|%pc|%sp)\\s*\\+\\s*(-?[0-9]+|0x[0-9A-F]+)\\],\\s*(%r(?:0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15)|%pc|%sp)\\s*(#.*)?$");
regex instrLdRegIndPomSym("^\\s*ld\\s+\\[(%r(?:0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15)|%pc|%sp)\\s*\\+\\s*([a-zA-Z][a-zA-Z0-9_]*)\\],\\s*(%r(?:0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15)|%pc|%sp)\\s*(#.*)?$");
