// -- IMPORTS

#include "CountedObject.h"

// -- IMPLEMENTATION

namespace Nexus
{
    #if ENABLE_ASSERTION
        uint64_t 
            CountedObject::OldCountedObjectIdentifier = 0;
        int64_t 
            CountedObject::CountedObjectCount = 0;
    #endif
}
