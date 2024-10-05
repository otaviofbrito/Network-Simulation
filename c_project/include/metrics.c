#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <float.h>

#include "metrics.h"
#include "little.h"

// gera um numero aleatorio distribuido uniformemente entre 0 e 1
double uniforme()
{
  double u = rand() / ((double)RAND_MAX + 1);
  // u == 0 --> ln(u) <-- problema
  // limitando u entre (0,1]
  u = 1.0 - u;
  return u;
}

// gera o tempo por meio de uma distribuicao exponencial
double gera_tempo(double l)
{
  return (-1.0 / l) * log(uniforme());
}

Metrics *new_metrics()
{
  Metrics *new = (Metrics *)malloc(sizeof(Metrics));
  if (new == NULL)
  {
    return NULL;
  }

  new->en_final = 0.0;
  new->ew_final = 0.0;
  new->lambda = 0.0;
  new->mu = 0.0;
  new->little_error = 0.0;
  new->ocupacao = 0.0;

  return new;
}

void update_metrics(Metrics *m, Little *en, Little *ew_chegadas, Little *ew_saidas, double soma_ocupacao, double tempo_decorrido)
{
  m->en_final = en->soma_areas / tempo_decorrido;
  m->ew_final = (ew_chegadas->soma_areas - ew_saidas->soma_areas) / ew_chegadas->num_eventos;
  m->lambda = ew_chegadas->num_eventos / tempo_decorrido;
  m->mu = (1 / m->ew_final) + m->lambda;
  m->ocupacao = soma_ocupacao / tempo_decorrido;
  m->little_error = abs(m->en_final - (m->lambda * m->ew_final));
}

// Simula navegação na web
int gerar_pacote()
{
  int random_value = rand() % 100; // Gera um número aleatório entre 0 e 99

  if (random_value < 50)
  {
    // 50% de chance de retornar 550 bytes
    return 550;
  }
  else if (random_value < 90)
  {
    // 40% de chance de retornar 40 bytes
    return 40;
  }
  else
  {
    // 10% de chance de retornar 1500 bytes
    return 1500;
  }
}

double gera_tempo_transmissao(double link)
{
  // L/R
  return gerar_pacote() / link;
}