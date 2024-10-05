#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <float.h>

#include "metrics.h"
#include "little.h"

#define INTERVALO 100.0
#define CSV_PATH "data/output.csv"

int main(int argc, char *argv[])
{
  if (argc != 5)
  {
    printf("Usage: %s seed parametro_chegada tamanho_link tempo_simulacao\n", argv[0]);
    return EXIT_FAILURE;
  }

  unsigned int seed = atoi(argv[1]);
  double parametro_chegada = atof(argv[2]);
  double tamanho_link = atof(argv[3]);
  double tempo_simulacao = atof(argv[4]);

  const double param1 = parametro_chegada;
  const double param2 = tamanho_link;

  srand(seed);

  parametro_chegada = 1.0 / parametro_chegada;
  
  double tempo_decorrido = 0.0;

  double tempo_chegada = gera_tempo(parametro_chegada);
  double tempo_saida = DBL_MAX;

  unsigned long int fila = 0;
  unsigned long int fila_max = 0;

  double soma_ocupacao = 0.0;

  // iniciar variaveis de Little
  Little *en = new_little();
  Little *ew_chegadas = new_little();
  Little *ew_saidas = new_little();

  // metricas
  Metrics *metrics = new_metrics();

  // Tempo inicial de calculo
  double tempo_calc = INTERVALO;

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
    tempo_decorrido = min(min(tempo_chegada, tempo_saida), tempo_calc);

    if (tempo_decorrido == tempo_chegada)
    {
      if (!fila)
      {
        tempo_saida = tempo_decorrido + gera_tempo_transmissao(tamanho_link);
        soma_ocupacao += tempo_saida - tempo_decorrido;
      }
      fila++;
      fila_max = fila > fila_max ? fila : fila_max;

      tempo_chegada = tempo_decorrido + gera_tempo(parametro_chegada);

      // Little
      littles_calc(en, tempo_decorrido);
      littles_calc(ew_chegadas, tempo_decorrido);
      en->num_eventos++;
      ew_chegadas->num_eventos++;
    }
    else if (tempo_decorrido == tempo_saida)
    {
      fila--;
      tempo_saida = DBL_MAX;
      if (fila)
      {
        tempo_saida = tempo_decorrido + gera_tempo_transmissao(tamanho_link);
        soma_ocupacao += tempo_saida - tempo_decorrido;
      }

      // Little
      littles_calc(en, tempo_decorrido);
      littles_calc(ew_saidas, tempo_decorrido);
      en->num_eventos--;
      ew_saidas->num_eventos++;
    }
    // Calcula métricas
    else
    {
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
              metrics->little_error, param1, param2);

      // Atualizar o próximo tempo para calcular métricas
      tempo_calc += INTERVALO;
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

  sprintf(new_name, "data/output_%s_%d.csv", time_str, file_ocup);

  if (rename(CSV_PATH, new_name) != 0)
  {
    perror("Error renaming file");
  }

  return EXIT_SUCCESS;
}