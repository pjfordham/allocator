#include <map>
#include <functional>
#include <cstdint>
#include <iostream>

struct alloc_info_t {
   std::function<void(void*)> destructor;
   std::size_t count;
   std::uintptr_t tagA;
   std::uintptr_t tagB;
   std::size_t sizeoftype;
};

std::map<uintptr_t, alloc_info_t> memory;

template<typename Type>
Type* z_allocate_and_construct( std::size_t count, std::uintptr_t tagA, std::uintptr_t tagB) {
   alloc_info_t info;
   info.sizeoftype = sizeof(Type);
   auto ptr = malloc( info.sizeoftype * count );

   // manually call constructors
   for (int i = 0; i< count ;i++ ) {
      new ((char*)ptr + info.sizeoftype * i) Type;
   }
   info.destructor = [] ( void *ptr ) { reinterpret_cast<Type*>(ptr)->~Type(); } ;
   info.count = count;
   info.tagA = tagA;
   info.tagB = tagB;
   memory[ (uintptr_t)ptr ] = info;
   return static_cast<Type*>(ptr);
}

template<typename Type>
void z_free( Type *mem ) {
   alloc_info_t info = memory[ (uintptr_t)mem ];

   for (int i = 0; i< info.count ;i++ ) {
      info.destructor( (char*)mem + info.sizeoftype * i );
   }
   free(mem);
}

void z_free_all_with_tag(  std::uintptr_t tagA ){
   for (auto element : memory) {
      auto ptr = element.first;
      alloc_info_t &info = element.second;
      if (info.tagA == tagA) {
         z_free( (void*)ptr);
      }
   }
}
