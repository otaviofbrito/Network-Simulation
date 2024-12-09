#include "event.h"
#include "heap.h"
#include "little.h"

Event newEvent(EnumEvent tipo, double tempo)
{
    Event evento;
    evento.tipo = tipo;
    evento.tempo = tempo;
    return evento;
}

Event insertNewEvent(MinHeap *heap, EnumEvent tipo, double tempo)
{
    Event new_event = newEvent(tipo, tempo);
    insertMinHeap(heap, new_event);
    return new_event;
}

