#pragma once

typedef struct Little
{
  unsigned long int num_eventos;
  double tempo_anterior;
  double soma_areas;
} Little;

void littles_calc(Little *e, const double current_time);

Little *new_little();
