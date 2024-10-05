import subprocess
import webbrowser
import os
from gen_report import *

report_path = 'report/report.html'

#Executa o projeto c com os parametros passados como entrada
def run_simulation(seed, l, m, sim_time):
    cmd = f'c_project/main {seed} {l} {m} {sim_time}'
    subprocess.run(cmd, shell=True)

#Abre automaticamente o arquivo html gerado no navegador
def open_report():
    if os.path.exists(report_path):
        webbrowser.open(f'{report_path}')
    else:
        print(f"O arquivo {report_path} não foi encontrado.")


def main():
    print()
    print("+" + "-" * 58 + "+")
    print("|" + " " * 12 + ">>Simulador de Eventos Discretos<<" + " " * 12 + "|")
    print("+" + "-" * 58 + "+\n")

    while True:
        try:
            num_simulations = int(input("Número de simulações: "))
            if num_simulations <= 0:
                raise ValueError(
                    "O número de simulações deve ser maior que zero.")
            break  # Se a entrada for válida, saia do loop
        except ValueError as e:
            print(f"Entrada inválida: {e}. Tente novamente.")

    for i in range(num_simulations):
        while True:
            try:
                seed = input(f"\nSeed aleatória[{i+1}]: ")
                if not seed.isdigit():
                    raise ValueError("A seed deve ser um número inteiro.")
                tx_chegada = float(
                    input(f"Informe a taxa de chegada dos pacotes (pacotes/s):[{i+1}]: "))
                if tx_chegada < 0:
                    raise ValueError(
                        "A taxa de chegada dos pacotes deve ser não negativo.")
                tx_saida = float(
                    input(f"Informe o tamanho do link (bytes/s): [{i+1}]: "))
                if tx_saida < 0:
                    raise ValueError(
                        "O tamanho do link deve ser não negativo.")
                tempo = int(input(f"Tempo de simulação (s)[{i+1}]: "))
                if tempo <= 0:
                    raise ValueError(
                        "O tempo de simulação deve ser maior que zero.")
                run_simulation(int(seed), tx_chegada, tx_saida, tempo)
                break
            except ValueError as e:
                print(f"Entrada inválida: {e}. Tente novamente.")

    generate_report()
    open_report()


if __name__ == '__main__':
    main()