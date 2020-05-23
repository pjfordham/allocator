#include "allocator.hh"
#include <fmt/format.h>

struct A {
   int x;
   int y;
   A(): x(0), y(0) {
      fmt::print(" A(): {:x}\n", (uintptr_t)this );
   }
   ~A() {
      fmt::print("~A(): {:x}\n", (uintptr_t)this );
   }
};

struct B {
   int x;
   int y;
   B(): x(0), y(0) {
      fmt::print(" B(): {:x}\n", (uintptr_t)this );
   }
   ~B() {
      fmt::print("~B(): {:x}\n", (uintptr_t)this );
   }
};

struct C {
   int x;
   int y;
};

int main(int argc, const char** argv) {

   A* a = z_allocate_and_construct<A>(3, 1, reinterpret_cast<uintptr_t>(&a));
   B* b = z_allocate_and_construct<B>(5, 1,  reinterpret_cast<uintptr_t>(&b));
   A* c = z_allocate_and_construct<A>(2, 2,  reinterpret_cast<uintptr_t>(&c));
   B* d = z_allocate_and_construct<B>(4, 3,  reinterpret_cast<uintptr_t>(&d));

   C* e = z_allocate_and_construct<C>(4, 3,  reinterpret_cast<uintptr_t>(&d));

   z_dump();
   z_free_all_with_tag( 1 );
   z_free( c );
   z_dump();
   z_free( d );
   z_free( e );
   z_dump();


   return 0;
}
