#include <stdio.h>

#include "xlang/x_actor.h"
#include "xlang/x_align.h"
#include "xlang/x_allocatormanager.h"
#include "xlang/x_framework.h"

#include "xlang/x_linearallocator.h"


namespace Example
{


// A simple actor that requires aligned memory allocation.
class AlignedActor : public Theron::Actor
{
public:

    inline AlignedActor()
    {
        printf("AlignedActor instantiated at memory address 0x%p\n", this);
    }
};


} // namespace Example


// Notify Theron of the alignment requirements of the actor class. This causes
// Theron to request memory of the correct alignment when allocating
// instances of the actor class. Here we request 128-byte alignment, just as
// a test. On some platforms this is the size of a cache line.
// 
THERON_ALIGN_ACTOR(Example::AlignedActor, 128);


int main()
{
    // Construct a LinearAllocator around a memory buffer. The DefaultAllocator used
    // by Theron by default supports alignment, so can be safely used. Here we show
    // the use of a custom allocator, which must also support alignment, as an example.
    const unsigned int BUFFER_SIZE(16384); 
    unsigned char buffer[BUFFER_SIZE];
    Example::LinearAllocator linearAllocator(buffer, BUFFER_SIZE);

    // Set the custom allocator.
    Theron::AllocatorManager::Instance().SetAllocator(&linearAllocator);

    // Instantiate the actor so it prints out its alignment.
    Theron::Framework framework;
    Theron::ActorRef actor(framework.CreateActor<Example::AlignedActor>());

    printf("Finished\n");
    return 0;
}

