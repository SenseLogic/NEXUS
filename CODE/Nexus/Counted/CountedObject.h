#pragma once

// -- IMPORTS

#include <cstdint>

#include "Engine/Core/Memory/Memory.h"

// -- NAMESPACE

namespace Nexus
{
    // -- TYPES

    struct CountedObject
    {
        // -- ATTRIBUTES

        uint64_t
            IsAllocated : 1,
            IsDestructed : 1,
            ReferenceCount : 20,
            LinkCount : 21,
            PointerCount : 21;
        #if ENABLE_ASSERTION
            uint64_t
                CountedObjectIdentifier;
            static uint64_t
                OldCountedObjectIdentifier;
            static int64_t
                CountedObjectCount;
        #endif

        // -- CONSTRUCTORS

        inline CountedObject(
            ) :
            IsAllocated( 0 ),
            IsDestructed( 0 ),
            ReferenceCount( 0 ),
            LinkCount( 0 ),
            PointerCount( 0 )
        {
            #if ENABLE_ASSERTION
                CountedObjectIdentifier
                    = OldCountedObjectIdentifier * 6364136223846793005ULL + 1442695040888963407ULL;

                OldCountedObjectIdentifier = CountedObjectIdentifier;
                ++CountedObjectCount;
            #endif
        }

        // ~~

        inline CountedObject(
            const CountedObject &
            ) :
            CountedObject()
        {
        }

        // -- DESTRUCTORS

        inline virtual ~CountedObject(
            )
        {
            ASSERT( ReferenceCount == 0 );

            IsDestructed = 1;

            #if ENABLE_ASSERTION
                CountedObjectIdentifier = 0;
                --CountedObjectCount;
            #endif
        }

        // -- OPERATORS

        inline CountedObject & operator=(
            const CountedObject &
            )
        {
            return *this;
        }

        // -- OPERATIONS

        inline void ReleaseMemory(
            )
        {
            if ( IsAllocated )
            {
                ASSERT(
                    ReferenceCount == 0
                    && LinkCount == 0
                    && IsDestructed
                    );

                IsAllocated = 0;
                Allocator::Free( this );
            }
        }

        // ~~

        virtual void Destruct(
            )
        {
            this->~CountedObject();
        }

        // ~~

        inline void IncrementReferenceCounter(
            )
        {
            ASSERT(
                IsAllocated
                && !IsDestructed
                );

            ++ReferenceCount;
        }

        // ~~

        inline void DecrementReferenceCounter(
            )
        {
            ASSERT(
                IsAllocated
                && !IsDestructed
                && ReferenceCount > 0
                );

            --ReferenceCount;

            if ( ReferenceCount == 0 )
            {
                if ( LinkCount == 0
                     && PointerCount == 0 )
                {
                    Delete( this );
                }
                else
                {
                    Destruct();

                    if ( LinkCount == 0
                         && PointerCount == 0 )
                    {
                        ReleaseMemory();
                    }
                }
            }
        }

        // ~~

        inline void IncrementLinkCounter(
            )
        {
            ASSERT(
                IsAllocated
                && !IsDestructed
                );

            ++LinkCount;
        }

        // ~~

        inline void DecrementLinkCounter(
            )
        {
            ASSERT(
                IsAllocated
                && LinkCount > 0
                );

            --LinkCount;

            if ( LinkCount == 0
                 && ReferenceCount == 0
                 && PointerCount == 0
                 && IsDestructed )
            {
                ReleaseMemory();
            }
        }

        // ~~

        inline void IncrementPointerCounter(
            )
        {
            ASSERT(
                IsAllocated
                && !IsDestructed
                );

            ++PointerCount;
        }

        // ~~

        inline void DecrementPointerCounter(
            )
        {
            ASSERT(
                IsAllocated
                && PointerCount > 0
                );

            --PointerCount;

            if ( PointerCount == 0
                 && LinkCount == 0
                 && ReferenceCount == 0
                 && IsDestructed )
            {
                ReleaseMemory();
            }
        }
    };
}

// -- FUNCTIONS

#define DECLARE_COUNTED_TYPE( _Type_ ) \
    \
    virtual void Destruct( \
        ) override \
    { \
        this->~_Type_(); \
    }
