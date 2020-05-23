#include <map>
#include <cstdint>
#include <cstdlib>
#include <type_traits>
#include <fmt/format.h>

// TODO:
// specialize on single item alloc
// specialize on tagAless, hence no lambda
// support debugging mode where all static type are check against dynamic info
// constexpr
// exceptions?

class Allocator {

   struct alloc_info_t {
      typedef void (*destructor_t) ( void *mem, std::size_t count);
      bool trivially_destructible;
      std::size_t count;
      std::uintptr_t tagA;
      std::uintptr_t tagB;
      std::size_t sizeoftype;
      destructor_t destructor;
      // could have some string based on __func__ to see typename
      // or https://en.cppreference.com/w/cpp/types/type_index
      alloc_info_t(bool td,
                   std::size_t c,
                   std::uintptr_t A,
                   std::uintptr_t B,
                   std::size_t sot,
                   destructor_t d) : trivially_destructible(td), count(c), tagA(A), tagB(B), sizeoftype(sot), destructor(d) {}
   };

   std::map<uintptr_t, alloc_info_t> memory;

public:
   Allocator() = default;
   Allocator( const Allocator& alloc ) = delete;
   Allocator( Allocator&& alloc ) = delete;

   template<typename Type>
   Type* malloc( std::size_t count, std::uintptr_t tagA, std::uintptr_t tagB) {

      // Do the actuall memory allocation
      auto ptr = static_cast<Type*>( std::malloc( sizeof(Type) * count ) );

      // Run the constructors
      if (!std::is_trivially_constructible<Type>::value) {
            for (int i = 0; i < count ;i++ ) {
               new (ptr + i) Type;
            }
         }

      // Setup the destructor lambda
      alloc_info_t::destructor_t destructor = std::is_trivially_destructible<Type>::value ? (alloc_info_t::destructor_t)nullptr :
         [] ( void *mem, std::size_t count ) {
         auto ptr = static_cast<Type*>(mem);
         for (int i = 0; i < count ;i++ ) {
            (ptr + i)->~Type();
         }
      };

      // Emplace the pointer and the allocaion information block into the map
      memory.emplace( std::piecewise_construct,
                      std::make_tuple( (uintptr_t) ptr),
                      std::make_tuple(std::is_trivially_destructible<Type>::value,
                                      count,
                                      tagA,
                                      tagB,
                                      sizeof(Type),
                                      destructor) );

      return ptr;
   }

   template<typename Type>
   void free( Type *mem ) {

      // Run the destructor directly
      if (!std::is_trivially_destructible<Type>::value) {
         // We should validate the find here else we will crash for unknown pointers
         const alloc_info_t &info = memory.find( (uintptr_t)mem )->second;
         for (int i = 0; i < info.count ;i++ ) {
            (mem + i)->~Type();
         }
      }

      // Free the underlying memory
      std::free(mem);

      // Remove the entry from the map
      memory.erase( (uintptr_t)mem );

   }

   void free_tags( std::uintptr_t low_tag, std::uintptr_t high_tag ) {

      // Walk over the map
      for (auto element : memory) {
         auto ptr = element.first;
         const alloc_info_t &info = element.second;

         // If the tag matches free the allocation
         if ( low_tag <= info.tagA && info.tagA <= high_tag ) {

            // Run the destructors lambda
            if ( !info.trivially_destructible ) {
               info.destructor( (void*)ptr, info.count );
            }

            // Free the underlying memory
            std::free((void*)ptr);

            // Remove the entry from the map
            memory.erase( ptr );
         }
      }

   }

   void dump_heap( std::uintptr_t low_tag, std::uintptr_t high_tag ) const {
      fmt::print( "{:>16} {:>16} {:>16} {:>16} {:>16} {:>16} \n",
                  "Pointer", "No Destructor", "Count",
                  "tagA", "tagB","size of type");
      for (auto element : memory) {
         auto ptr = element.first;
         alloc_info_t &info = element.second;
         // If the tag matches dump the info
         if ( low_tag <= info.tagA && info.tagA <= high_tag ) {
            fmt::print( "{:>16x} {:>16} {:>16} {:>16} {:>16x} {:>16} \n",
                        ptr, info.trivially_destructible, info.count,
                        info.tagA, info.tagB, info.sizeoftype);
         }
      }
   }

};
