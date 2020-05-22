#include "allocator.hh"
#include <iostream>

struct A {
   int x;
   int y;
   A(): x(0), y(0) {
      std::cout << "A()  " << (uintptr_t)this<< "\n";
   }
   ~A() {
      std::cout << "~A() " << (uintptr_t)this<< "\n";
   }
};

struct B {
   int x;
   int y;
   B(): x(0), y(0) {
      std::cout << "B()  " << (uintptr_t)this<< "\n";
   }
   ~B() {
      std::cout << "~B() " << (uintptr_t)this<< "\n";
   }
};

int main(int argc, const char** argv) {

   A* a = z_allocate_and_construct<A>(3, 1, reinterpret_cast<uintptr_t>(&a));
   B* b = z_allocate_and_construct<B>(5, 1,  reinterpret_cast<uintptr_t>(&b));
   A* c = z_allocate_and_construct<A>(2, 2,  reinterpret_cast<uintptr_t>(&c));
   B* d = z_allocate_and_construct<B>(4, 3,  reinterpret_cast<uintptr_t>(&d));
   
   z_free_all_with_tag( 1 );
   z_free( c );
   z_free( d );
   
   return 0;
}

