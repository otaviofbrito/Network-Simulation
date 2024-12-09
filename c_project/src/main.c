#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <float.h>

#include "metrics.h"
#include "little.h"
#include "heap.h"
#include "event.h"

#define INTERVALO 100.0
#define MAX_HEAP_SIZE 5000
#define CSV_PATH "c_project/data/output.csv"

int main(int argc, char *argv[])
{
  if (argc != 8)
  {
    printf("Usage: %s seed taxa_chegada_web intervalo_entre_chamadas duracao_chamada intervalo_pacotes tamanho_link tempo_simulacao\n", argv[0]);
    return EXIT_FAILURE;
  }

  unsigned int seed = atoi(argv[1]);
  double lambda_web = atof(argv[2]);
  double lambda_call = 1.0 / atof(argv[3]);
  double mu_call = 1.0 / atof(argv[4]);
  double lambda_pacote_call = (1.0 / atof(argv[5])) * (lambda_call / mu_call);
  double tamanho_link = atof(argv[6]);

  double tempo_simulacao = atof(argv[7]);

  srand(seed);
  MinHeap *heap = createMinHeap(MAX_HEAP_SIZE);

  double tempo_decorrido = 0.0;
  double tempo_saida = 0.0;
  double last_time = 0.0;

  double soma_ocupacao = 0.0;

  unsigned long int fila = 0;
  unsigned long int fila_call = 0;
  unsigned long int fila_max = 0;

  Event saida_call;
  Event saida_web;
  Event evento;
  // Little total = WEB+CALL
  Little *en = new_little();
  Little *ew_chegadas = new_little();
  Little *ew_saidas = new_little();

  // Little CALL
  Little *en_call = new_little();
  Little *ew_chegadas_call = new_little();
  Little *ew_saidas_call = new_little();

  // metricas
  Metrics *metrics = new_metrics();
  Metrics *metrics_call = new_metrics();

  insertNewEvent(CHEGADA_PACOTE_WEB, gera_tempo(lambda_web), heap);
  insertNewEvent(COLETA, INTERVALO, heap);
  insertNewEvent(CHEGADA_PACOTE_CALL, gera_tempo(lambda_call), heap);

  // Criar arquivo csv
  FILE *file = fopen(CSV_PATH, "a");
  if (file == NULL)
  {
    printf("Error opening file!\n");
    return EXIT_FAILURE;
  }
  fprintf(file, "Time,Fila Max,Ocupacao,E[N],E[W],Lambda,Mu,Erro de Little,Lambda_Web,Lambda_Call,Mu_Call,Lambda_Pacote_Call,Link,Tipo\n");

  while (tempo_decorrido <= tempo_simulacao)
  {
    // Retira o proximo evento da heap
    evento = extractMin(heap);
    tempo_decorrido = evento.tempo;
    switch (evento.tipo)
    {
    case CHEGADA_PACOTE_WEB:

      if (!fila)
      {
        tempo_saida = gera_tempo_transmissao(0, tamanho_link);
        last_time = max(saida_call.tempo, tempo_decorrido);
        saida_web = insertNewEvent(SAIDA_PACOTE_WEB, last_time + tempo_saida, heap);
        soma_ocupacao += tempo_saida;
      }

      fila++;
      fila_max = fila > fila_max ? fila : fila_max;
      insertNewEvent(CHEGADA_PACOTE_WEB, tempo_decorrido + gera_tempo(lambda_web), heap);

      // Little
      littles_calc(en, tempo_decorrido);
      littles_calc(ew_chegadas, tempo_decorrido);
      en->num_eventos++;
      ew_chegadas->num_eventos++;
      break;

    case SAIDA_PACOTE_WEB:
      fila--;
      if (fila)
      {
        tempo_saida = gera_tempo_transmissao(0, tamanho_link);
        last_time = max(saida_call.tempo, tempo_decorrido);
        saida_web = insertNewEvent(SAIDA_PACOTE_WEB, last_time + tempo_saida, heap);
        soma_ocupacao += tempo_saida;
      }

      // Little
      littles_calc(en, tempo_decorrido);
      littles_calc(ew_saidas, tempo_decorrido);
      en->num_eventos--;
      ew_saidas->num_eventos++;
      break;

    case CHEGADA_PACOTE_CALL:
      if (!fila_call)
      {
        tempo_saida = gera_tempo_transmissao(1, tamanho_link);
        last_time = max(saida_web.tempo, tempo_decorrido);
        saida_call = insertNewEvent(SAIDA_PACOTE_CALL, last_time + tempo_saida, heap);
        soma_ocupacao += tempo_saida;
      }

      fila_call++;
      insertNewEvent(CHEGADA_PACOTE_CALL, tempo_decorrido + gera_tempo(lambda_pacote_call), heap);

      // Little
      littles_calc(en, tempo_decorrido);
      littles_calc(ew_chegadas, tempo_decorrido);
      en->num_eventos++;
      ew_chegadas->num_eventos++;

      littles_calc(en_call, tempo_decorrido);
      littles_calc(ew_chegadas_call, tempo_decorrido);
      en_call->num_eventos++;
      ew_chegadas_call->num_eventos++;
      break;

    case SAIDA_PACOTE_CALL:
      fila_call--;
      if (fila_call)
      {
        tempo_saida = gera_tempo_transmissao(1, tamanho_link);
        last_time = max(saida_web.tempo, tempo_decorrido);
        saida_call = insertNewEvent(SAIDA_PACOTE_CALL, last_time + tempo_saida, heap);
        soma_ocupacao += tempo_saida;
      }

      // Little
      littles_calc(en, tempo_decorrido);
      littles_calc(ew_saidas, tempo_decorrido);
      en->num_eventos--;
      ew_saidas->num_eventos++;

      littles_calc(en_call, tempo_decorrido);
      littles_calc(ew_saidas_call, tempo_decorrido);
      en_call->num_eventos--;
      ew_saidas_call->num_eventos++;
      break;

    case COLETA:

      // atualiza os 3 gráficos
      littles_calc(en, tempo_decorrido);
      littles_calc(ew_chegadas, tempo_decorrido);
      littles_calc(ew_saidas, tempo_decorrido);

      littles_calc(en_call, tempo_decorrido);
      littles_calc(ew_chegadas_call, tempo_decorrido);
      littles_calc(ew_saidas_call, tempo_decorrido);

      update_metrics(metrics, en, ew_chegadas, ew_saidas, soma_ocupacao, tempo_decorrido);
      update_metrics(metrics_call, en_call, ew_chegadas_call, ew_saidas_call, soma_ocupacao, tempo_decorrido);

      // Escrever no arquivo CSV
      fprintf(file, "%f,%lu,%f,%f,%f,%f,%f,%.15f,%f,%f,%f,%f,%f,%s\n",
              tempo_decorrido, fila_max, metrics->ocupacao,
              metrics->en_final, metrics->ew_final,
              metrics->lambda, metrics->mu,
              metrics->little_error, lambda_web, lambda_call,
              mu_call, lambda_pacote_call, tamanho_link, "WEB");

      fprintf(file, "%f,%lu,%f,%f,%f,%f,%f,%.15f,%f,%f,%f,%f,%f,%s\n",
              tempo_decorrido, fila_max, metrics_call->ocupacao,
              metrics_call->en_final, metrics_call->ew_final,
              metrics_call->lambda, metrics_call->mu,
              metrics_call->little_error, lambda_web, lambda_call,
              mu_call, lambda_pacote_call, tamanho_link, "CALL");

      // Atualizar o próximo tempo para calcular métricas
      insertNewEvent(COLETA, tempo_decorrido + INTERVALO, heap);
      break;
    }
  }
  fclose(file);

  littles_calc(ew_chegadas, tempo_decorrido);
  littles_calc(ew_saidas, tempo_decorrido);

  littles_calc(ew_chegadas_call, tempo_decorrido);
  littles_calc(ew_saidas_call, tempo_decorrido);

  update_metrics(metrics, en, ew_chegadas, ew_saidas, soma_ocupacao, tempo_decorrido);
  update_metrics(metrics_call, en_call, ew_chegadas_call, ew_saidas_call, soma_ocupacao, tempo_decorrido);

  printf("\nPACOTES WEB+CALL:\n");
  printf("Maior tamanho de fila alcancado: %ld\n", fila_max);
  printf("Ocupacao: %lf\n", metrics->ocupacao);
  printf("E[N]: %lf\n", metrics->en_final);
  printf("E[W]: %lf\n", metrics->ew_final);
  printf("Erro de Little: %e\n", metrics->little_error);
  printf("Lambda: %lf\n", metrics->lambda);
  printf("Mu: %lf\n", metrics->mu);

  printf("\n-------------------------------\n");

  printf("\nPACOTES CALL:\n");
  printf("E[N]: %lf\n", metrics_call->en_final);
  printf("E[W]: %lf\n", metrics_call->ew_final);
  printf("Erro de Little: %e\n", metrics_call->little_error);
  printf("Lambda: %lf\n", metrics_call->lambda);
  printf("Mu: %lf\n", metrics_call->mu);

  // Adicionar ocupacao e data ao nome do arquivo csv para identificação
  int file_ocup = (int)(round(metrics->ocupacao * 100));
  char new_name[50];

  // Obter o tempo atual
  time_t now = time(NULL);
  struct tm *tm_info = localtime(&now);

  // Formatar a data e hora
  char time_str[20];
  strftime(time_str, sizeof(time_str), "%Y%m%d_%H%M%S", tm_info);

  sprintf(new_name, "c_project/data/output_%s_%d.csv", time_str, file_ocup);

  if (rename(CSV_PATH, new_name) != 0)
  {
    perror("Error renaming file");
  }

  freeMinHeap(heap);
  return EXIT_SUCCESS;
}
