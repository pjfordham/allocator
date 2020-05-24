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
   float z;
   B(): x(0), y(0), z(2.3) {
      fmt::print(" B(): {:x}\n", (uintptr_t)this );
   }
   ~B() {
      fmt::print("~B(): {:x}\n", (uintptr_t)this );
   }
};

struct C {
   int x;
   int y;
   int z;
};

int main(int argc, const char** argv) {

   Allocator Z;

   A* a = Z.malloc<A>(3, 1, reinterpret_cast<uintptr_t>(&a));
   B* b = Z.malloc<B>(5, 1, reinterpret_cast<uintptr_t>(&b));
   A* c = Z.malloc<A>(2, 2, reinterpret_cast<uintptr_t>(&c));
   B* d = Z.malloc<B>(4, 3, reinterpret_cast<uintptr_t>(&d));
   C* e = Z.malloc<C>(4, 3, reinterpret_cast<uintptr_t>(&d));

   void *buffer = Z.malloc<void>(10,5, 0xDEADBEEF);

   Z.dump_heap(1,5);
   Z.free_tags( 1, 1 );
   Z.dump_heap(1,5);
   Z.free( c );
   Z.dump_heap(1,5);
   Z.free( d );
   Z.free( e );
   Z.dump_heap(1,5);
   Z.change_tag( buffer, 4 );
   Z.dump_heap(1,5);
   Z.free( buffer );
   Z.dump_heap(1,5);

   return 0;
}
