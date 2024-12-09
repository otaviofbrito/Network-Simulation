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
#define CSV_PATH "c_project/data/output.csv"

int main(int argc, char *argv[])
{
  // if (argc != 5)
  // {
  //   printf("Usage: %s seed taxa_chegada_web intervalo_entre_chamadas duracao_chamada intervalo_pacotes tempo_simulacao\n", argv[0]);
  //   return EXIT_FAILURE;
  // }

  unsigned int seed = 100;                                    // atoi(argv[1]);
  double lambda_web = 100;                                   // atof(argv[2]);
  double lambda_call = 1.0/30;                                    // atoi(argv[3]);
  double mu_call = 1.0/60;                                        // atoi(argv[4]);
  double lambda_pacote_call = (1.0/0.02) * (lambda_call/mu_call);//1.0/(0.02 / (mu_call / lambda_call)); // atoi(argv[5]);
  double tamanho_link = 100166.666667;                        // atof(argv[6]);
  double tempo_simulacao = 36000;                             // atof(argv[7]);

  srand(seed);
  MinHeap *heap = createMinHeap(5000);

  double tempo_decorrido = 0.0;
  double tempo_saida = 0.0;
  double last_time = 0.0;

  Event saida_call;
  Event saida_web;

  double soma_ocupacao = 0.0;

  unsigned long int fila = 0;
  unsigned long int fila_call = 0;
  unsigned long int fila_max = 0;

  Little *en = new_little();
  Little *ew_chegadas = new_little();
  Little *ew_saidas = new_little();

  // metricas
  Metrics *metrics = new_metrics();

  insertNewEvent(heap, CHEGADA_WEB, gera_tempo(lambda_web));
  insertNewEvent(heap, COLETA, 100.0);
  insertNewEvent(heap, CHEGADA_PACOTE_CALL, gera_tempo(lambda_call));

  // Criar arquivo csv
  FILE *file = fopen(CSV_PATH, "a");
  if (file == NULL)
  {
    printf("Error opening file!\n");
    return EXIT_FAILURE;
  }
  fprintf(file, "Time,Fila Max,Ocupacao,E[N],E[W],Lambda,Mu,Erro de Little,Chegada,Saida\n");

  while (tempo_decorrido <= tempo_simulacao)
  {
    Event current_event = extractMin(heap);
    tempo_decorrido = current_event.tempo;

    switch (current_event.tipo)
    {
    case CHEGADA_WEB:

      if (!fila)
      {
        tempo_saida = gera_tempo_transmissao(0, tamanho_link);
        last_time = max(saida_call.tempo, tempo_decorrido);
        saida_web = insertNewEvent(heap, SAIDA_WEB, last_time + tempo_saida);
        soma_ocupacao += tempo_saida;
      }

      fila++;
      fila_max = fila > fila_max ? fila : fila_max;
      insertNewEvent(heap, CHEGADA_WEB, tempo_decorrido + gera_tempo(lambda_web));

      // Little
      littles_calc(en, tempo_decorrido);
      littles_calc(ew_chegadas, tempo_decorrido);
      en->num_eventos++;
      ew_chegadas->num_eventos++;
      break;

    case SAIDA_WEB:
      fila--;
      if (fila)
      {
        tempo_saida = gera_tempo_transmissao(0, tamanho_link);
        last_time = max(saida_call.tempo, tempo_decorrido);
        saida_web = insertNewEvent(heap, SAIDA_WEB, last_time + tempo_saida);
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
        saida_call = insertNewEvent(heap, SAIDA_PACOTE_CALL, last_time + tempo_saida);
        soma_ocupacao += tempo_saida;
      }

      fila_call++;
      insertNewEvent(heap, CHEGADA_PACOTE_CALL, tempo_decorrido + gera_tempo(lambda_pacote_call));

      // Little
      littles_calc(en, tempo_decorrido);
      littles_calc(ew_chegadas, tempo_decorrido);
      en->num_eventos++;
      ew_chegadas->num_eventos++;
      break;

    case SAIDA_PACOTE_CALL:
      fila_call--;
      if (fila_call)
      {
        tempo_saida = gera_tempo_transmissao(1, tamanho_link);
        last_time = max(saida_web.tempo, tempo_decorrido);
        saida_call = insertNewEvent(heap, SAIDA_PACOTE_CALL, last_time + tempo_saida);
        soma_ocupacao += tempo_saida;
      }

      // Little
      littles_calc(en, tempo_decorrido);
      littles_calc(ew_saidas, tempo_decorrido);
      en->num_eventos--;
      ew_saidas->num_eventos++;
      break;

    case COLETA:

      // atualiza os 3 gráficos
      littles_calc(en, tempo_decorrido);
      littles_calc(ew_chegadas, tempo_decorrido);
      littles_calc(ew_saidas, tempo_decorrido);

      update_metrics(metrics, en, ew_chegadas, ew_saidas, soma_ocupacao, tempo_decorrido);

      // Escrever no arquivo CSV
      fprintf(file, "%f,%lu,%f,%f,%f,%f,%f,%f,%f,%f\n",
              tempo_decorrido, fila_max, metrics->ocupacao,
              metrics->en_final, metrics->ew_final,
              metrics->lambda, metrics->mu,
              metrics->little_error, 0.0, 0.0);

      // Atualizar o próximo tempo para calcular métricas
      insertNewEvent(heap, COLETA, tempo_decorrido + 100.00);
      break;
    }
  }
  fclose(file);

  littles_calc(ew_chegadas, tempo_decorrido);
  littles_calc(ew_saidas, tempo_decorrido);

  update_metrics(metrics, en, ew_chegadas, ew_saidas, soma_ocupacao, tempo_decorrido);

  printf("\nMaior tamanho de fila alcancado: %ld\n", fila_max);
  printf("Ocupacao: %lf\n", metrics->ocupacao);
  printf("E[N]: %lf\n", metrics->en_final);
  printf("E[W]: %lf\n", metrics->ew_final);
  printf("Erro de Little: %lf\n", metrics->little_error);
  printf("Lambda: %lf\n", metrics->lambda);
  printf("Mu: %lf\n", metrics->mu);

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