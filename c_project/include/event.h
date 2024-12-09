#pragma once

#include "heap.h"

// Definição do tipo de evento
typedef enum
{
  COLETA,
  CHEGADA_WEB,
  SAIDA_WEB,
  CHEGADA_PACOTE_CALL,
  SAIDA_PACOTE_CALL,
  CHEGADA_CALL,
  SAIDA_CALL
} EventType;

// Estrutura do evento
typedef struct Event
{
  EventType tipo; // Tipo do evento
  double tempo;   // Tempo associado ao evento
} Event;

Event newEvent(EventType tipo, double tempo);
Event insertNewEvent(MinHeap *heap, EventType tipo, double tempo);