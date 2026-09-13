// -- IMPORTS

#include "TestScript.h"

#include <cstring>
#include <cstdint>
#include <utility>
#include <vector>

#include "Engine/Core/Log.h"
#include "Engine/Core/Memory/Memory.h"
#include "CountedLink.h"
#include "CountedReference.h"
#include "CountedPointer.h"

using namespace Nexus;

// -- TYPES

struct Animal : public CountedObject
{
    DECLARE_COUNTED_TYPE( Animal );

    // -- ATTRIBUTES

    static int
        animalCount;

    // -- CONSTRUCTORS

    Animal(
        )
    {
        ++animalCount;
    }

    // -- DESTRUCTORS

    virtual ~Animal(
        )
    {
        --animalCount;
    }

    // -- INQUIRIES

    virtual const char * GetKind(
        ) const
    {
        return "Animal";
    }
};

// ~~

struct Mammal : public Animal
{
    DECLARE_COUNTED_TYPE( Mammal );

    // -- ATTRIBUTES

    static int
        mammalCount;

    // -- CONSTRUCTORS

    Mammal(
        )
    {
        ++mammalCount;
    }

    // -- DESTRUCTORS

    ~Mammal(
        ) override
    {
        --mammalCount;
    }

    // -- INQUIRIES

    const char * GetKind(
        ) const override
    {
        return "Mammal";
    }
};

// ~~

struct Dog : public Mammal
{
    DECLARE_COUNTED_TYPE( Dog );

    // -- ATTRIBUTES

    static int
        dogCount;

    // -- CONSTRUCTORS

    Dog(
        )
    {
        ++dogCount;
    }

    // -- DESTRUCTORS

    ~Dog(
        ) override
    {
        --dogCount;
    }

    // -- INQUIRIES

    const char * GetKind(
        ) const override
    {
        return "Dog";
    }
};

// ~~

struct Node : public CountedObject
{
    DECLARE_COUNTED_TYPE( Node );

    // -- ATTRIBUTES

    CountedLink<Node>
        superNode;
    CountedReference<Animal>
        animal;
    std::vector<CountedReference<Node>>
        subNodeArray;
    static int
        nodeCount;
    static CountedLink<Node>
        * HeldSuperNode;

    // -- CONSTRUCTORS

    Node(
        )
    {
        ++nodeCount;
    }

    // -- DESTRUCTORS

    ~Node(
        )
    {
        if ( HeldSuperNode != nullptr )
        {
            HeldSuperNode->SetNull();
            HeldSuperNode = nullptr;
        }

        --nodeCount;
    }
};

// -- VARIABLES

int
    Animal::animalCount = 0;
int
    Mammal::mammalCount = 0;
int
    Dog::dogCount = 0;
int
    Node::nodeCount = 0;
CountedLink<Node>
    * Node::HeldSuperNode = nullptr;

// -- FUNCTIONS

// ~~ CountedReference

static void TestReferenceSoleOwnerDeletesObject(
    )
{
    CountedReference<Dog> dog = NewCounted<Dog>();

    ASSERT( dog->ReferenceCount == 1 );
    ASSERT( dog->LinkCount == 0 );
    ASSERT( dog->PointerCount == 0 );
    ASSERT( Dog::dogCount == 1 );
    ASSERT( Mammal::mammalCount == 1 );
    ASSERT( Animal::animalCount == 1 );
    ASSERT( CountedObject::CountedObjectCount == 1 );

    dog.SetNull();

    ASSERT( Dog::dogCount == 0 );
    ASSERT( Mammal::mammalCount == 0 );
    ASSERT( Animal::animalCount == 0 );
    ASSERT( CountedObject::CountedObjectCount == 0 );
}

// ~~

static void TestReferenceCopyIncrementsRefCount(
    )
{
    CountedReference<Dog> dogA = NewCounted<Dog>();
    CountedReference<Dog> dogB = dogA;

    ASSERT( dogA->ReferenceCount == 2 );
    ASSERT( Dog::dogCount == 1 );
    ASSERT( CountedObject::CountedObjectCount == 1 );

    dogA.SetNull();
    ASSERT( dogB->ReferenceCount == 1 );
    ASSERT( Dog::dogCount == 1 );

    dogB.SetNull();
    ASSERT( Dog::dogCount == 0 );
    ASSERT( CountedObject::CountedObjectCount == 0 );
}

// ~~

static void TestReferenceReassignWhileObjectAlive(
    )
{
    CountedReference<Dog> dogA = NewCounted<Dog>();
    CountedReference<Dog> dogB = NewCounted<Dog>();
    CountedReference<Dog> countedReference = dogA;

    ASSERT( Dog::dogCount == 2 );
    ASSERT( dogA->ReferenceCount == 2 );
    ASSERT( dogB->ReferenceCount == 1 );
    ASSERT( CountedObject::CountedObjectCount == 2 );

    countedReference = dogB;

    ASSERT( dogA->ReferenceCount == 1 );
    ASSERT( dogB->ReferenceCount == 2 );
    ASSERT( !countedReference.IsNull() );

    countedReference.SetNull();
    dogA.SetNull();
    dogB.SetNull();
    ASSERT( Dog::dogCount == 0 );
    ASSERT( CountedObject::CountedObjectCount == 0 );
}

// ~~

static void TestReferenceDefaultIsNull(
    )
{
    CountedReference<Dog> countedReference;

    ASSERT( countedReference.IsNull() );
    ASSERT( countedReference.GetAddress() == nullptr );
    ASSERT( CountedObject::CountedObjectCount == 0 );
}

// ~~

static void TestReferencePolymorphicAnimalHierarchy(
    )
{
    CountedReference<Animal> animal = NewCounted<Dog>();

    ASSERT( Animal::animalCount == 1 );
    ASSERT( Mammal::mammalCount == 1 );
    ASSERT( Dog::dogCount == 1 );
    ASSERT( strcmp( animal->GetKind(), "Dog" ) == 0 );
    ASSERT( CountedObject::CountedObjectCount == 1 );

    CountedReference<Animal> copiedAnimal = animal;

    ASSERT( animal->ReferenceCount == 2 );
    ASSERT( strcmp( copiedAnimal->GetKind(), "Dog" ) == 0 );

    animal.SetNull();
    ASSERT( Dog::dogCount == 1 );

    copiedAnimal.SetNull();
    ASSERT( Dog::dogCount == 0 );
    ASSERT( CountedObject::CountedObjectCount == 0 );
}

// ~~ CountedPointer

static void TestPointerFromLiveReference(
    )
{
    CountedReference<Dog> dog = NewCounted<Dog>();
    CountedPointer<Dog> sameDog = dog;

    ASSERT( dog->ReferenceCount == 1 );
    ASSERT( dog->PointerCount == 1 );
    ASSERT( !sameDog.IsNull() );
    ASSERT( sameDog.operator Dog *() != nullptr );
    ASSERT( strcmp( sameDog->GetKind(), "Dog" ) == 0 );

    sameDog.SetNull();
    ASSERT( dog->PointerCount == 0 );
    dog.SetNull();
    ASSERT( Dog::dogCount == 0 );
    ASSERT( CountedObject::CountedObjectCount == 0 );
}

// ~~

static void TestPointerReleaseReferenceAfterPointerReleased(
    )
{
    CountedReference<Dog> dog = NewCounted<Dog>();
    CountedPointer<Dog> sameDog = dog;

    ASSERT( Dog::dogCount == 1 );
    ASSERT( dog->PointerCount == 1 );

    sameDog.SetNull();
    ASSERT( dog->PointerCount == 0 );

    dog.SetNull();
    ASSERT( Dog::dogCount == 0 );
    ASSERT( CountedObject::CountedObjectCount == 0 );
}

// ~~

static void TestPointerMultiplePointers(
    )
{
    CountedReference<Dog> dog = NewCounted<Dog>();
    CountedPointer<Dog> pointerA = dog;
    CountedPointer<Dog> pointerB = dog;

    ASSERT( dog->PointerCount == 2 );
    ASSERT( Dog::dogCount == 1 );

    pointerA.SetNull();
    ASSERT( dog->PointerCount == 1 );
    ASSERT( !pointerB.IsNull() );

    pointerB.SetNull();
    ASSERT( dog->PointerCount == 0 );

    dog.SetNull();
    ASSERT( Dog::dogCount == 0 );
    ASSERT( CountedObject::CountedObjectCount == 0 );
}

// ~~

static void TestPointerReassignWhileObjectAlive(
    )
{
    CountedReference<Dog> dogA = NewCounted<Dog>();
    CountedReference<Dog> dogB = NewCounted<Dog>();
    CountedPointer<Dog> pointer = dogA;

    ASSERT( dogA->PointerCount == 1 );
    ASSERT( dogB->PointerCount == 0 );

    pointer = dogB;

    ASSERT( dogA->PointerCount == 0 );
    ASSERT( dogB->PointerCount == 1 );
    ASSERT( !pointer.IsNull() );

    pointer.SetNull();
    dogA.SetNull();
    dogB.SetNull();
    ASSERT( Dog::dogCount == 0 );
    ASSERT( CountedObject::CountedObjectCount == 0 );
}

// ~~

static void TestPointerNeverCreatedNoPointerCount(
    )
{
    CountedReference<Dog> dog = NewCounted<Dog>();

    ASSERT( Dog::dogCount == 1 );
    ASSERT( dog->PointerCount == 0 );

    dog.SetNull();
    ASSERT( Dog::dogCount == 0 );
    ASSERT( CountedObject::CountedObjectCount == 0 );
}

// ~~

static void TestPointerCopyIncrementsPointerCount(
    )
{
    CountedReference<Dog> dog = NewCounted<Dog>();
    CountedPointer<Dog> pointerA = dog;
    CountedPointer<Dog> pointerB = pointerA;

    ASSERT( dog->PointerCount == 2 );

    pointerA.SetNull();
    ASSERT( dog->PointerCount == 1 );

    pointerB.SetNull();
    ASSERT( dog->PointerCount == 0 );

    dog.SetNull();
    ASSERT( Dog::dogCount == 0 );
    ASSERT( CountedObject::CountedObjectCount == 0 );
}

// ~~

static void TestPointerSurvivesDeferredDestruction(
    )
{
    CountedReference<Dog> dog = NewCounted<Dog>();
    CountedLink<Dog> link = dog;
    CountedPointer<Dog> pointer = dog;

    ASSERT( dog->ReferenceCount == 1 );
    ASSERT( dog->LinkCount == 1 );
    ASSERT( dog->PointerCount == 1 );

    dog.SetNull();
    ASSERT( Dog::dogCount == 0 );
    ASSERT( CountedObject::CountedObjectCount == 0 );
    ASSERT( !link.IsNull() );
    ASSERT( !pointer.IsNull() );

    pointer.SetNull();
    link.SetNull();
    ASSERT( pointer.IsNull() );
    ASSERT( link.IsNull() );
    ASSERT( CountedObject::CountedObjectCount == 0 );
}

// ~~ CountedLink

static void TestLinkDefersMemoryUntilCleared(
    )
{
    CountedReference<Dog> dog = NewCounted<Dog>();
    CountedLink<Dog> link = dog;

    ASSERT( dog->ReferenceCount == 1 );
    ASSERT( dog->LinkCount == 1 );
    ASSERT( Dog::dogCount == 1 );

    dog.SetNull();
    ASSERT( Dog::dogCount == 0 );
    ASSERT( CountedObject::CountedObjectCount == 0 );
    ASSERT( !link.IsNull() );

    link.SetNull();
    ASSERT( link.IsNull() );
    ASSERT( CountedObject::CountedObjectCount == 0 );
}

// ~~

static void TestLinkClearedDuringDestructionFreesMemory(
    )
{
    CountedReference<Node> node = NewCounted<Node>();
    CountedLink<Node> link = node;

    Node::HeldSuperNode = &link;

    ASSERT( node->ReferenceCount == 1 );
    ASSERT( node->LinkCount == 1 );
    ASSERT( CountedObject::CountedObjectCount == 1 );

    node.SetNull();

    ASSERT( link.IsNull() );
    ASSERT( Node::HeldSuperNode == nullptr );
    ASSERT( Node::nodeCount == 0 );
    ASSERT( CountedObject::CountedObjectCount == 0 );
}

// ~~

static void TestLinkCopyIncrementsLinkCount(
    )
{
    CountedReference<Dog> dog = NewCounted<Dog>();
    CountedLink<Dog> linkA = dog;
    CountedLink<Dog> linkB = linkA;

    ASSERT( dog->LinkCount == 2 );

    linkA.SetNull();
    ASSERT( dog->LinkCount == 1 );

    linkB.SetNull();
    ASSERT( dog->LinkCount == 0 );

    dog.SetNull();
    ASSERT( Dog::dogCount == 0 );
    ASSERT( CountedObject::CountedObjectCount == 0 );
}

// ~~

static void TestLinkReassignWhileObjectAlive(
    )
{
    CountedReference<Dog> dogA = NewCounted<Dog>();
    CountedReference<Dog> dogB = NewCounted<Dog>();
    CountedLink<Dog> link = dogA;

    ASSERT( dogA->LinkCount == 1 );
    ASSERT( dogB->LinkCount == 0 );
    ASSERT( CountedObject::CountedObjectCount == 2 );

    link = dogB;

    ASSERT( dogA->LinkCount == 0 );
    ASSERT( dogB->LinkCount == 1 );
    ASSERT( !link.IsNull() );

    link.SetNull();
    dogA.SetNull();
    dogB.SetNull();
    ASSERT( Dog::dogCount == 0 );
    ASSERT( CountedObject::CountedObjectCount == 0 );
}

// ~~

static void TestLinkNeverCreatedNoLinkCount(
    )
{
    CountedReference<Dog> dog = NewCounted<Dog>();

    ASSERT( Dog::dogCount == 1 );
    ASSERT( dog->LinkCount == 0 );

    dog.SetNull();
    ASSERT( Dog::dogCount == 0 );
    ASSERT( CountedObject::CountedObjectCount == 0 );
}

// ~~ Node graph

static void TestNodeGraphParentChildReferences(
    )
{
    CountedReference<Node> parent = NewCounted<Node>();
    CountedReference<Node> child = NewCounted<Node>();

    child->superNode = parent;
    parent->subNodeArray.push_back( child );

    ASSERT( Node::nodeCount == 2 );
    ASSERT( parent->ReferenceCount == 1 );
    ASSERT( parent->LinkCount == 1 );
    ASSERT( child->ReferenceCount == 2 );
    ASSERT( child->superNode.GetAddress() == parent.GetAddress() );
    ASSERT( CountedObject::CountedObjectCount == 2 );

    child.SetNull();
    ASSERT( parent->subNodeArray[ 0 ]->ReferenceCount == 1 );
    ASSERT( parent->LinkCount == 1 );

    parent->subNodeArray.clear();
    ASSERT( Node::nodeCount == 1 );
    ASSERT( parent->LinkCount == 0 );

    parent.SetNull();
    ASSERT( Node::nodeCount == 0 );
    ASSERT( CountedObject::CountedObjectCount == 0 );
}

// ~~

static void TestNodeAnimalPolymorphism(
    )
{
    CountedReference<Node> node = NewCounted<Node>();
    CountedReference<Animal> dog = NewCounted<Dog>();

    node->animal = dog;

    ASSERT( Node::nodeCount == 1 );
    ASSERT( Dog::dogCount == 1 );
    ASSERT( strcmp( node->animal->GetKind(), "Dog" ) == 0 );
    ASSERT( dog->ReferenceCount == 2 );
    ASSERT( CountedObject::CountedObjectCount == 2 );

    CountedPointer<Animal> animalPointer = node->animal;

    ASSERT( dog->PointerCount == 1 );
    ASSERT( strcmp( animalPointer->GetKind(), "Dog" ) == 0 );

    animalPointer.SetNull();
    node->animal.SetNull();
    dog.SetNull();
    node.SetNull();

    ASSERT( Node::nodeCount == 0 );
    ASSERT( Dog::dogCount == 0 );
    ASSERT( CountedObject::CountedObjectCount == 0 );
}

// ~~

static void TestNodeDeferredDestructionWithChildren(
    )
{
    CountedReference<Node> parent = NewCounted<Node>();
    CountedReference<Node> child = NewCounted<Node>();
    CountedLink<Node> parentLink = parent;
    CountedPointer<Node> parentPointer = parent;

    child->superNode = parent;
    parent->subNodeArray.push_back( child );

    parent.SetNull();
    ASSERT( Node::nodeCount == 1 );
    ASSERT( CountedObject::CountedObjectCount == 1 );
    ASSERT( !child->superNode.IsNull() );
    ASSERT( !parentLink.IsNull() );
    ASSERT( !parentPointer.IsNull() );

    child->superNode.SetNull();
    ASSERT( !parentLink.IsNull() );

    parentLink.SetNull();
    ASSERT( parentLink.IsNull() );

    parentPointer.SetNull();
    child.SetNull();
    ASSERT( Node::nodeCount == 0 );
    ASSERT( CountedObject::CountedObjectCount == 0 );
}

// ~~ Combined

static void TestReferenceLinkPointerTogether(
    )
{
    CountedReference<Dog> dog = NewCounted<Dog>();
    CountedLink<Dog> link = dog;
    CountedPointer<Dog> pointer = dog;

    ASSERT( dog->ReferenceCount == 1 );
    ASSERT( dog->LinkCount == 1 );
    ASSERT( dog->PointerCount == 1 );

    link.SetNull();
    pointer.SetNull();
    ASSERT( dog->LinkCount == 0 );
    ASSERT( dog->PointerCount == 0 );

    dog.SetNull();
    ASSERT( Dog::dogCount == 0 );
    ASSERT( CountedObject::CountedObjectCount == 0 );
}

// ~~

static void TestDeferredDestructionClearsLinkThenPointer(
    )
{
    CountedReference<Dog> dog = NewCounted<Dog>();
    CountedLink<Dog> link = dog;
    CountedPointer<Dog> pointer = dog;

    dog.SetNull();
    ASSERT( Dog::dogCount == 0 );
    ASSERT( CountedObject::CountedObjectCount == 0 );

    link.SetNull();
    ASSERT( link.IsNull() );

    pointer.SetNull();
    ASSERT( pointer.IsNull() );
    ASSERT( CountedObject::CountedObjectCount == 0 );
}

// ~~

static bool RunAllCountedTests(
    )
{
    ASSERT( CountedObject::CountedObjectCount == 0 );

    TestReferenceSoleOwnerDeletesObject();
    TestReferenceCopyIncrementsRefCount();
    TestReferenceReassignWhileObjectAlive();
    TestReferenceDefaultIsNull();
    TestReferencePolymorphicAnimalHierarchy();

    TestPointerFromLiveReference();
    TestPointerReleaseReferenceAfterPointerReleased();
    TestPointerMultiplePointers();
    TestPointerReassignWhileObjectAlive();
    TestPointerNeverCreatedNoPointerCount();
    TestPointerCopyIncrementsPointerCount();
    TestPointerSurvivesDeferredDestruction();

    TestLinkDefersMemoryUntilCleared();
    TestLinkClearedDuringDestructionFreesMemory();
    TestLinkCopyIncrementsLinkCount();
    TestLinkReassignWhileObjectAlive();
    TestLinkNeverCreatedNoLinkCount();

    TestNodeGraphParentChildReferences();
    TestNodeAnimalPolymorphism();
    TestNodeDeferredDestructionWithChildren();

    TestReferenceLinkPointerTogether();
    TestDeferredDestructionClearsLinkThenPointer();

    ASSERT( CountedObject::CountedObjectCount == 0 );

    return true;
}

// ~~

TestScript::TestScript(
    const SpawnParams& params
    ) :
    Script( params )
{
}

// ~~

void TestScript::OnEnable(
    )
{
    if ( RunAllCountedTests() )
    {
        LOG( Info, "All counted tests passed" );
    }
    else
    {
        LOG( Error, "One or more counted tests failed" );
    }
}
