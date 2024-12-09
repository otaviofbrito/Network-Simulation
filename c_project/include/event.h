#pragma once

#include "heap.h"

// Definição do tipo de evento
typedef enum
{
  CHEGADA_PACOTE_WEB,
  SAIDA_PACOTE_WEB,
  CHEGADA_PACOTE_CALL,
  SAIDA_PACOTE_CALL,
  COLETA
} EnumEvent;

// Estrutura do evento
typedef struct Event
{
  EnumEvent tipo; // Tipo do evento
  double tempo;   // Tempo associado ao evento
} Event;

Event newEvent(EnumEvent tipo, double tempo);
Event insertNewEvent(EnumEvent tipo, double tempo, MinHeap *heap);