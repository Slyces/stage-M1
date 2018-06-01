#include "messages.h"
#include <stdlib.h>

physicalMessage * PhysicalCreate(int sender, int receiver,
       messageType type, void * content) {
    physicalMessage * physMsg = malloc(sizeof(physicalMessage));
    physMsg->sender = sender;
    physMsg->receiver = receiver;
    physMsg->type = type;
    physMsg->content = content;
    return physMsg;
}

void PhysicalDestroy(physicalMessage * msg) {
    free(msg);
}
