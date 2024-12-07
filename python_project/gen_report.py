import plotly.express as px
import plotly.graph_objects as go
from jinja2 import Template
import pandas as pd
import os

output_html_path = r"report/report2.html"
input_template_path = r"template/template.html"
csv_path = r"c_project/data/"

# Carrega os arquivos csv gerados no projeto c como um pandas dataframe


def load_csv():
    i = 1
    arquivos_csv = [f for f in os.listdir(csv_path) if f.endswith('.csv')]
    dataframes = []
    for arquivo in arquivos_csv:
        caminho_arquivo = os.path.join(csv_path, arquivo)
        df = pd.read_csv(caminho_arquivo)
        if not df.empty:
            number = arquivo.split('output')[-1].split('.')[0][-2:]
            params = f"(P1:{df.iloc[0]['Chegada']}, P2:{df.iloc[0]['Saida']})"
            df['Fonte'] = f'{i}: {params} = {number}%'
            i += 1
        dataframes.append(df)

    return dataframes

# Injeta os plots dentro do template HTML


def render_to_html(figs):
    # Convert figures to HTML
    plotly_jinja_data = {
        "fig1": figs[0].to_html(full_html=False, include_plotlyjs='cdn'),
        "fig2": figs[1].to_html(full_html=False, include_plotlyjs='cdn'),
        "fig3": figs[2].to_html(full_html=False, include_plotlyjs='cdn'),
        "figx": figs[3].to_html(full_html=False, include_plotlyjs='cdn'),
        "figy": figs[4].to_html(full_html=False, include_plotlyjs='cdn'),
        "fig4": figs[5].to_html(full_html=False, include_plotlyjs='cdn'),
        "fig5": figs[6].to_html(full_html=False, include_plotlyjs='cdn')}

    # Render template with multiple plots
    with open(output_html_path, "w", encoding="utf-8") as output_file:
        with open(input_template_path) as template_file:
            j2_template = Template(template_file.read())
            output_file.write(j2_template.render(plotly_jinja_data))

# "Time,Fila Max,Ocupacao,E[N],E[W],Lambda,Mu,Erro de Little,Chegada,Saida\n"


def create_table(dfs):
    fig = go.Figure()

    # Adiciona uma tabela para cada dataframe
    for i, df in enumerate(dfs):
        fig.add_trace(
            go.Table(
                header=dict(
                    values=["Tempo", "Máximo de<br>Fila", "Ocupação",
                            "E[N]", "E[W]", "Lambda",
                            "Mu", "Erro de<br>Little", "Taxa de<br>Chegada<br>(pcts/s)", "Tamanho do<br>Link<br>(bytes/s)"],
                    font=dict(size=10),
                    align="left"
                ),
                cells=dict(
                    values=[df[k].tolist() for k in df.columns[:-1]],
                    align="left"
                )
            )
        )

    # Cria o layout com menu dropdown
    fig.update_layout(
        updatemenus=[
            dict(
                buttons=[dict(
                    label=f"Tabela {i + 1}",
                    method="update",
                    args=[
                        {"visible": [
                            False] * len(dfs), "visible": [i == j for j in range(len(dfs))]},
                        {"title.text": f"Tabela {dfs[i].iloc[0]['Fonte']}"}]
                ) for i in range(len(dfs))],
                direction="down",
                showactive=True
            )
        ],
        showlegend=False,
        title_text=f"Tabela {dfs[0].iloc[0]['Fonte']}"
    )

    # Inicialmente, mostra apenas a primeira tabela
    fig.update_traces(visible=False)
    fig.data[0].visible = True

    return fig

# Cria as visualizações gráficas com o plotly e no final injeta no HTML


def generate_report():
    dfs = load_csv()
    df = pd.concat(dfs, ignore_index=True)
    figs = []

    figs.append(create_table(dfs))

    fig = px.line(df, x='Time', y="Ocupacao",
                  color='Fonte', title='Utilização (Ocupação)')
    fig.update_layout(
        xaxis_title='Tempo',
        yaxis_title='Ocupação',
        legend_title='Parâmetros'
    )
    figs.append(fig)

    fig = px.line(df, x='Time', y=[ "E[N]",
                        "E[W]"], color='Fonte', title='E[N] & E[W]')
    fig.update_layout(
        xaxis_title='Tempo',
        yaxis_title='E[N] & E[W]',
        legend_title='Parâmetros'
    )
    figs.append(fig)


    fig = px.line(df, x='Time', y=[
                        "E[N]"], color='Fonte', title='E[N]')
    fig.update_layout(
        xaxis_title='Tempo',
        yaxis_title='E[N]',
        legend_title='Parâmetros'
    )
    figs.append(fig)

    fig = px.line(df, x='Time', y=["E[W]"], color='Fonte', title='E[W]')
    fig.update_layout(
        xaxis_title='Tempo',
        yaxis_title='E[W]',
        legend_title='Parâmetros'
    )
    figs.append(fig)

    fig = px.line(df, x='Time', y=["Lambda", "Mu"], color='Fonte')
    fig.update_layout(
        xaxis_title='Tempo',
        yaxis_title='Taxas',
        legend_title='Parâmetros'
    )
    figs.append(fig)

    fig = px.line(df, x='Time', y="Erro de Little",
                  color='Fonte', title='Erro de Little')
    fig.update_layout(
        xaxis_title='Tempo',
        yaxis_title='Erro',
        legend_title='Parâmetros'
    )
    figs.append(fig)

    render_to_html(figs)

    return
