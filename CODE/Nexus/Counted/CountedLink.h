#pragma once

// -- IMPORTS

#include "CountedObject.h"

// -- TYPES

namespace Nexus
{
    template <typename T>
    struct CountedLink
    {
        // -- ATTRIBUTES

        T
            * Address;
        #if ENABLE_ASSERTION
            uint64_t
                CountedObjectIdentifier;
        #endif

        // -- CONSTRUCTORS

        inline CountedLink(
            ) :
            Address( nullptr )
        {
            #if ENABLE_ASSERTION
                CountedObjectIdentifier = 0;
            #endif
        }

        // ~~

        inline CountedLink(
            const CountedLink & countedLink
            ) :
            Address( nullptr )
        {
            SetAddress( countedLink.Address );
        }

        // ~~

        inline CountedLink(
            T * address
            ) :
            Address( nullptr )
        {
            SetAddress( address );
        }

        // ~~

        inline ~CountedLink(
            )
        {
            SetNull();
        }

        // -- OPERATORS

        inline CountedLink & operator=(
            const CountedLink & countedLink
            )
        {
            SetAddress( countedLink.Address );

            return *this;
        }

        // ~~

        inline CountedLink & operator=(
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
                    && Address->LinkCount > 0
                    && ( Address->IsDestructed
                         || Address->CountedObjectIdentifier == CountedObjectIdentifier )
                    );

                Address->DecrementLinkCounter();
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
                    non_const_address->IncrementLinkCounter();
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
