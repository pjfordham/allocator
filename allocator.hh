#include <map>
#include <functional>
#include <cstdint>
#include <type_traits>
#include <fmt/format.h>

struct alloc_info_t {
   bool trivially_destructible;
   std::size_t count;
   std::uintptr_t tagA;
   std::uintptr_t tagB;
   std::size_t sizeoftype;
   std::function<void(void*)> destructor;
   // could have some string based on __func__ to see typename
   // or https://en.cppreference.com/w/cpp/types/type_index
};

std::map<uintptr_t, alloc_info_t> memory;

template<typename Type>
Type* z_allocate_and_construct( std::size_t count, std::uintptr_t tagA, std::uintptr_t tagB) {
   alloc_info_t info {
      std::is_trivially_destructible<Type>::value,
         count,
         tagA,
         tagB,
         sizeof(Type) };

   auto ptr = static_cast<Type*>( malloc( info.sizeoftype * count ) );

   // This is probably redundant.
   if (!std::is_trivially_constructible<Type>::value) {
      // manually call constructors
      for (int i = 0; i< count ;i++ ) {
         new (ptr + i) Type;
      }
   } else {
      fmt::print( " Trivial(): {:x}\n", (std::uintptr_t)ptr );
   }

   if ( !info.trivially_destructible) {
      info.destructor = [=, count = info.count] ( void *mem) {
         auto ptr = static_cast<Type*>(mem);
         for (int i = 0; i< count ;i++ ) {
            (ptr + i)->~Type();
         }
      } ;
   }
   memory[ (uintptr_t)ptr ] = info;
   return ptr;
}

template<typename Type>
void z_free( Type *mem ) {
   alloc_info_t info = memory[ (uintptr_t)mem ];

   if (!info.trivially_destructible) {
      info.destructor( (void*)mem);
   } else {
      fmt::print( "~Trivial(): {:x}\n", (std::uintptr_t)mem );
   }
   free(mem);
   memory.erase( (uintptr_t)mem );
}

static void z_free_all_with_tag(  std::uintptr_t tagA ){
   for (auto element : memory) {
      auto ptr = element.first;
      alloc_info_t &info = element.second;
      if (info.tagA == tagA) {
         z_free( (void*)ptr);
      }
   }
}

static void z_dump( ){
   fmt::print( "{:>16} {:>16} {:>16} {:>16} {:>16} {:>16} \n",
               "Pointer", "No Destructor", "Count",
               "tagA", "tagB","size of type");
   for (auto element : memory) {
      auto ptr = element.first;
      alloc_info_t &info = element.second;
      fmt::print( "{:>16x} {:>16} {:>16} {:>16} {:>16x} {:>16} \n",
                  ptr, info.trivially_destructible, info.count,
                  info.tagA, info.tagB, info.sizeoftype);
   }
}
