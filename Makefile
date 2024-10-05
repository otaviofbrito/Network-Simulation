.PHONY: all c_project python_script clean setup

all: env c_project python_script

auto: env c_project python_input

purge: clean remove_env

env: 
	@if [ ! -d ".env" ]; then \
		echo "[WARNING]: O ambiente virtual não foi criado. Execute 'make build' primeiro!"; \
		exit 1; \
	fi
	
c_project:
	$(MAKE) -C c_project

python_script:
	. .env/bin/activate; python3 python_project/main.py; deactivate

python_input:
	. .env/bin/activate; python3 python_project/main.py < input.txt; deactivate

clean:
	$(MAKE) -C c_project clean
	@if [ -f report/report.html ]; then rm report/report.html; fi
	@echo "[OK]: Ambiente limpo!"

build:
	@echo "[INFO]: Criando ambiente virtual..."
	python3 -m venv .env
	@echo "[INFO]: Instalando dependências no ambiente virtual ..."
	. .env/bin/activate; pip install -r requirements.txt; deactivate
	@echo "[OK]: Ambiente virtual criado!"

remove_env:
	@echo "[INFO]: Excluindo ambiente virtual..."
	rm -rf .env
	@echo "[OK]: Ambiente virtual removido!"