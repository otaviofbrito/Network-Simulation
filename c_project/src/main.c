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

double maximo(double num1, double num2)
{
  if (num1 > num2)
  {
    return num1;
  }
  return num2;
}


int main(int argc, char *argv[])
{
  // Capacity of 10 elements
  MinHeap *heap = createMinHeap(2000);
  double tempo_simulacao = 36000;
  double tempo_decorrido = 0.0;

  double lambda_call = 30;
  double mu_call = 60;

  double lambda_pacote_call = 0.02;
  double lambda_web = 0.01;
  double tamanho_link;
  double tempo_saida;

  Event saida_call;
  Event saida_web;
  Event chamada;

  double soma_ocupacao = 0.0;

  unsigned long int fila = 0;
  unsigned long int fila_call = 0;
  unsigned long int fila_max = 0;

  // iniciar variaveis de Little
  Little *en = new_little();
  Little *ew_chegadas = new_little();
  Little *ew_saidas = new_little();

  // metricas
  Metrics *metrics = new_metrics();

  srand(100);
  // calculando chances que cada pacote tem de ser gerado
  tamanho_link = 100166.666667;

  chamada = insertNewEvent(heap, CHEGADA_CALL, gera_tempo(lambda_call));
  insertNewEvent(heap, SAIDA_CALL, chamada.tempo + gera_tempo(mu_call));
  insertNewEvent(heap, CHEGADA_WEB, gera_tempo(lambda_web));
  insertNewEvent(heap, CHEGADA_PACOTE_CALL, chamada.tempo);
  insertNewEvent(heap, COLETA, 100.0);

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
        saida_web = insertNewEvent(heap, SAIDA_WEB, maximo(saida_call.tempo, tempo_decorrido) + tempo_saida);
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
        saida_web = insertNewEvent(heap, SAIDA_WEB, maximo(saida_call.tempo, tempo_decorrido) + tempo_saida);
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
        saida_call = insertNewEvent(heap, SAIDA_PACOTE_CALL, maximo(saida_web.tempo, tempo_decorrido) + tempo_saida);
        soma_ocupacao += tempo_saida;
      }

      fila_call++;

      insertNewEvent(heap, CHEGADA_PACOTE_CALL, tempo_decorrido + gera_tempo(lambda_pacote_call / (mu_call / lambda_call)));
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
        saida_call = insertNewEvent(heap, SAIDA_PACOTE_CALL, maximo(saida_web.tempo, tempo_decorrido) + tempo_saida);
        soma_ocupacao += tempo_saida;
      }

      // Little
      littles_calc(en, tempo_decorrido);
      littles_calc(ew_saidas, tempo_decorrido);
      en->num_eventos--;
      ew_saidas->num_eventos++;
      break;

    case CHEGADA_CALL:
      chamada = insertNewEvent(heap, CHEGADA_CALL, tempo_decorrido + gera_tempo(lambda_call));
      insertNewEvent(heap, SAIDA_CALL, chamada.tempo + gera_tempo(mu_call));
      break;

    case SAIDA_CALL:
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
