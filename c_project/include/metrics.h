#pragma once
#include "little.h"

#define min(a, b) ((a) < (b) ? (a) : (b))

typedef struct Metrics
{
  double en_final;
  double ew_final;
  double lambda;
  double mu;
  double little_error;
  double ocupacao;
} Metrics;

double uniforme();

double gera_tempo(double l);

int gerar_pacote();

double gera_tempo_transmissao(double link);

Metrics *new_metrics();

void update_metrics(Metrics *m, Little *en, Little *ew_chegadas, Little *ew_saidas, double soma_ocupacao, double tempo_decorrido);
