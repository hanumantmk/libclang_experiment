#include <vector>

#define TAG __attribute__((annotate("tag")))

class TAG MyClass {
   public:
      int a TAG;
      int b;
      std::vector<int> c TAG;
};

class TAG MyClass2 {
   public:
      char a TAG;
      void foo() TAG;
};
