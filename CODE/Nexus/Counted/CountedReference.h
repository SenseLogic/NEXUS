#pragma once

#include <utility>

#include "CountedObject.h"

// -- TYPES

namespace Nexus
{
    template <typename T>
    struct CountedReference
    {
        // -- ATTRIBUTES

        T
            * Address;
        #if ENABLE_ASSERTION
            uint64_t
                CountedObjectIdentifier;
        #endif

        // -- CONSTRUCTORS

        inline CountedReference(
            ) :
            Address( nullptr )
        {
            #if ENABLE_ASSERTION
                CountedObjectIdentifier = 0;
            #endif
        }

        // ~~

        inline CountedReference(
            const CountedReference & countedReference
            ) :
            Address( nullptr )
        {
            SetAddress( countedReference.Address );
        }

        // ~~

        inline CountedReference(
            T * address
            ) :
            Address( nullptr )
        {
            SetAddress( address );
        }

        // ~~

        inline ~CountedReference(
            )
        {
            SetNull();
        }

        // -- OPERATORS

        inline CountedReference & operator=(
            const CountedReference & countedReference
            )
        {
            SetAddress( countedReference.Address );

            return *this;
        }

        // ~~

        inline CountedReference & operator=(
            const T * address
            )
        {
            SetAddress( address );

            return *this;
        }

        // ~~

        inline operator T *(
            void
            ) const
        {
            ASSERT(
                Address == nullptr
                || ( Address->IsAllocated
                     && !Address->IsDestructed
                     && Address->ReferenceCount > 0
                     && Address->CountedObjectIdentifier == CountedObjectIdentifier )
                );

            return Address;
        }

        // ~~

        inline T & operator*(
            void
            ) const
        {
            ASSERT(
                Address != nullptr
                && Address->IsAllocated
                && !Address->IsDestructed
                && Address->ReferenceCount > 0
                && Address->CountedObjectIdentifier == CountedObjectIdentifier
                );

            return *Address;
        }

        // ~~

        inline T * operator->(
            void
            ) const
        {
            ASSERT(
                Address != nullptr
                && Address->IsAllocated
                && !Address->IsDestructed
                && Address->ReferenceCount > 0
                && Address->CountedObjectIdentifier == CountedObjectIdentifier
                );

            return Address;
        }

        // -- INQUIRIES

        inline T * GetAddress(
            ) const
        {
            if ( Address == nullptr )
            {
                return nullptr;
            }
            else
            {
                ASSERT(
                    Address->IsAllocated
                    && !Address->IsDestructed
                    && Address->ReferenceCount > 0
                    && Address->CountedObjectIdentifier == CountedObjectIdentifier
                    );

                return Address;
            }
        }

        // ~~

        inline bool IsNull(
            ) const
        {
            return Address == nullptr;
        }

        // -- OPERATIONS

        void SetNull(
            )
        {
            if ( Address != nullptr )
            {
                ASSERT(
                    Address->IsAllocated
                    && !Address->IsDestructed
                    && Address->ReferenceCount > 0
                    && Address->CountedObjectIdentifier == CountedObjectIdentifier
                    );

                Address->DecrementReferenceCounter();
                Address = nullptr;

                #if ENABLE_ASSERTION
                    CountedObjectIdentifier = 0;
                #endif
            }
        }

        // ~~

        void SetAddress(
            const T * address
            )
        {
            T
                * non_const_address;

            if ( Address != address )
            {
                if ( address == nullptr )
                {
                    SetNull();
                }
                else
                {
                    non_const_address = ( T * )address;
                    non_const_address->IncrementReferenceCounter();
                    SetNull();
                    Address = non_const_address;

                    #if ENABLE_ASSERTION
                        CountedObjectIdentifier = address->CountedObjectIdentifier;
                    #endif
                }
            }
        }
    };

    // -- FUNCTIONS

    template<class T, class MemoryAllocator = Allocator>
    inline CountedReference<T> NewCounted(
        )
    {
        T
            * countedObject;

        countedObject = ( T * )MemoryAllocator::Allocate( sizeof( T ) );
        Memory::ConstructItem( countedObject );

        countedObject->IsAllocated = 1;

        return countedObject;
    }

    // ~~

    template<class T, class MemoryAllocator = Allocator, class... Args>
    inline CountedReference<T> NewCounted(
        Args&&...args
        )
    {
        T
            * countedObject;

        countedObject = ( T * )MemoryAllocator::Allocate( sizeof( T ) );

        new( countedObject ) T( std::forward<Args>( args )... );

        countedObject->IsAllocated = 1;

        return countedObject;
    }
}
