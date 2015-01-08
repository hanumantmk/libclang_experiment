#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include "clang-c/Index.h"

class Type {
   public:
   std::map<std::string, std::string> fields;
};

struct context {
   std::map<std::string, Type> types;
   std::map<std::string, Type>::iterator last_type;
   const char *name;
};


enum CXChildVisitResult
visitor (CXCursor cursor, CXCursor parent, CXClientData client_data) {
   CXFile file;
   unsigned line;
   unsigned column;
   context *ctx = (context *)client_data;
   const char *lf_name = ctx->name;

   CXSourceRange range = clang_getCursorExtent(parent);
   CXSourceLocation location = clang_getRangeStart(range);

   clang_getFileLocation(location, &file, &line, &column, 0);

   CXString filename = clang_getFileName(file);
   CXString name = clang_getCursorSpelling(parent);

   if (clang_getCString(filename) && strcmp(lf_name, clang_getCString(filename)) == 0) {
      if (clang_getCursorKind(cursor) == CXCursor_AnnotateAttr) {
         if (clang_getCursorKind(parent) == CXCursor_ClassDecl) {
            ctx->types[clang_getCString(name)];
            ctx->last_type = ctx->types.find(clang_getCString(name));
         } else {
            CXString type = clang_getTypeSpelling(clang_getCursorType(parent));
            ctx->last_type->second.fields[clang_getCString(name)] = clang_getCString(type);
            clang_disposeString(type);
         }
      }
   }

   clang_disposeString(filename);
   clang_disposeString(name);

   return CXChildVisit_Recurse;
}

void dump(int argc, char ** argv) {
   CXIndex index = clang_createIndex(0, 0);

   CXTranslationUnit tu = clang_parseTranslationUnit(index, argv[0], argv + 1, argc - 1, 0, 0, CXTranslationUnit_None);

   unsigned diagnosticCount = clang_getNumDiagnostics(tu);
   context ctx;

   ctx.name = argv[0];

   if (diagnosticCount) {
      std::cout << "diags: " << diagnosticCount << std::endl;

      for (int i = 0; i < diagnosticCount; i++) {
         CXDiagnostic diagnostic = clang_getDiagnostic(tu, i);
         CXSourceLocation location = clang_getDiagnosticLocation(diagnostic);

         unsigned line;
         unsigned column;

         clang_getSpellingLocation(location, 0, &line, &column, 0);

         CXString text = clang_getDiagnosticSpelling(diagnostic);

         std::cout << line << ":" << column << ": " << clang_getCString(text) << std::endl;

         clang_disposeString(text);
      }
   } else {
      clang_visitChildren(clang_getTranslationUnitCursor(tu), visitor, &ctx);

      for (auto && x : ctx.types) {
         std::cout << x.first << " {\n";
         for (auto && y : x.second.fields) {
            std::cout << y.first << " : " << y.second << "\n";
         }
         std::cout << "}\n";
      }
   }
}

int main(int argc, char ** argv) {
   dump(argc -1, argv + 1);

   return 0;
}
