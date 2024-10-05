#include <stdlib.h>

#include "little.h"

void littles_calc(Little *e, const double current_time)
{
  e->soma_areas += (current_time - e->tempo_anterior) * e->num_eventos;
  e->tempo_anterior = current_time;
}

Little *new_little()
{
  Little *new = (Little *)malloc(sizeof(Little));
  if (new == NULL)
  {
    return NULL;
  }

  new->num_eventos = 0;
  new->soma_areas = 0.0;
  new->tempo_anterior = 0.0;

  return new;
}
