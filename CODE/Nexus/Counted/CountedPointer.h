#pragma once

// -- IMPORTS

#include "CountedObject.h"

// -- TYPES

namespace Nexus
{
    template <typename T>
    struct CountedPointer
    {
        // -- ATTRIBUTES

        T
            * Address;
        #if ENABLE_ASSERTION
            uint64_t
                CountedObjectIdentifier;
        #endif

        // -- CONSTRUCTORS

        inline CountedPointer(
            ) :
            Address( nullptr )
        {
            #if ENABLE_ASSERTION
                CountedObjectIdentifier = 0;
            #endif
        }

        // ~~

        inline CountedPointer(
            const CountedPointer & countedPointer
            ) :
            Address( nullptr )
        {
            SetAddress( countedPointer.Address );
        }

        // ~~

        inline CountedPointer(
            T * address
            ) :
            Address( nullptr )
        {
            SetAddress( address );
        }

        // ~~

        inline ~CountedPointer(
            )
        {
            SetNull();
        }

        // -- OPERATORS

        inline CountedPointer & operator=(
            const CountedPointer & countedPointer
            )
        {
            SetAddress( countedPointer.Address );

            return *this;
        }

        // ~~

        inline CountedPointer & operator=(
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
                    && Address->PointerCount > 0
                    && ( Address->IsDestructed
                         || Address->CountedObjectIdentifier == CountedObjectIdentifier )
                    );

                Address->DecrementPointerCounter();
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
                    non_const_address->IncrementPointerCounter();
                    SetNull();
                    Address = non_const_address;

                    #if ENABLE_ASSERTION
                        CountedObjectIdentifier = address->CountedObjectIdentifier;
                    #endif
                }
            }
        }
    };
}
