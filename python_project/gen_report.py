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
            params = f"(R:{df.iloc[0]['Link']}Bytes/s)"
            df['Fonte'] = f'{i}: {params} - {number}%'
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
        "fig4": figs[3].to_html(full_html=False, include_plotlyjs='cdn'),
        "fig5": figs[4].to_html(full_html=False, include_plotlyjs='cdn'),
        "fig6": figs[5].to_html(full_html=False, include_plotlyjs='cdn'),
        "fig7": figs[6].to_html(full_html=False, include_plotlyjs='cdn'),
        "fig8": figs[7].to_html(full_html=False, include_plotlyjs='cdn'),
        "fig9": figs[8].to_html(full_html=False, include_plotlyjs='cdn'),
        "fig10": figs[9].to_html(full_html=False, include_plotlyjs='cdn'),
        "fig11": figs[10].to_html(full_html=False, include_plotlyjs='cdn'),
        "fig12": figs[11].to_html(full_html=False, include_plotlyjs='cdn')}

    # Render template with multiple plots
    with open(output_html_path, "w", encoding="utf-8") as output_file:
        with open(input_template_path) as template_file:
            j2_template = Template(template_file.read())
            output_file.write(j2_template.render(plotly_jinja_data))

# "Time,Fila Max,Ocupacao,E[N],E[W],Lambda,Mu,Erro de Little,Lambda_Web, Lambda_Call, Mu_Call, Lambda_Pacote_Call, Link, Tipo"


def create_table(dfs, columns, _values, _type):
    fig = go.Figure()

    # Adiciona uma tabela para cada dataframe
    for i, df in enumerate(dfs):
        df = df.loc[df["Tipo"] == _type]
        df = df[columns]
        fig.add_trace(
            go.Table(
                header=dict(
                    values=_values,
                    font=dict(size=10),
                    align="left"
                ),
                cells=dict(
                    values=[df[k].tolist() for k in df.columns[:]],
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

    dict_web = {
        'Time': 'Tempo',
        'Fila Max': 'Máximo de<br>Fila',
        'Ocupacao': 'Ocupação',
        'E[N]': 'E[N]',
        'E[W]': 'E[W]',
        'Lambda': 'Lambda',
        'Mu': 'Mu',
        'Erro de Little': 'Erro de<br>Little',
        'Link': 'Tamanho do<br>Link<br>(bytes/s)',
        'Tipo': 'Tipo<br>Do<br>Pacote'
    }
    # FIG 1 - Tabela 1 CALL+WEB
    figs.append(create_table(dfs, list(dict_web.keys()),
                list(dict_web.values()), "WEB+CALL"))

    dict_call = {
        'Time': 'Tempo',
        'E[N]': 'E[N]',
        'E[W]': 'E[W]',
        'Lambda': 'Lambda',
        'Mu': 'Mu',
        'Erro de Little': 'Erro de<br>Little',
        'Link': 'Tamanho do<br>Link<br>(bytes/s)',
        'Tipo': 'Tipo<br>Do<br>Pacote'
    }
    # FIG 2 - Tabela 2 CALL
    figs.append(create_table(dfs, list(dict_call.keys()),
                list(dict_call.values()), "CALL"))

    df_web = df.loc[df["Tipo"] == "WEB+CALL"]
    df_call = df.loc[df["Tipo"] == "CALL"]

    # FIG 3 - GRAFICO 1 WEB
    fig = px.line(df_web, x='Time', y="Ocupacao",
                  color='Fonte', title='Utilização (Ocupação)')
    fig.update_layout(
        xaxis_title='Tempo',
        yaxis_title='Ocupação',
        legend_title='Parâmetros'
    )

    figs.append(fig)

    # FIG 4 - GRAFICO 2 WEB
    fig = px.line(df_web, x='Time', y=["E[N]",
                                       "E[W]"], color='Fonte', title='E[N] & E[W] - Total')
    fig.update_layout(
        xaxis_title='Tempo',
        yaxis_title='E[N] & E[W]',
        legend_title='Parâmetros'
    )
    figs.append(fig)

    # FIG 5 - GRAFICO 3 WEB
    fig = px.line(df_web, x='Time', y=[
        "E[N]"], color='Fonte', title='E[N] - Total')
    fig.update_layout(
        xaxis_title='Tempo',
        yaxis_title='E[N]',
        legend_title='Parâmetros'
    )
    figs.append(fig)

    # FIG 6 - GRAFICO 4 WEB
    fig = px.line(df_web, x='Time', y=["E[W]"], color='Fonte', title='E[W] - Total')
    fig.update_layout(
        xaxis_title='Tempo',
        yaxis_title='E[W]',
        legend_title='Parâmetros'
    )
    figs.append(fig)

    # FIG 7 - GRAFICO 1 CALL
    fig = px.line(df_call, x='Time', y=["E[N]",
                                        "E[W]"], color='Fonte', title='E[N] & E[W] - Chamada em Tempo Real')
    fig.update_layout(
        xaxis_title='Tempo',
        yaxis_title='E[N] & E[W]',
        legend_title='Parâmetros'
    )
    figs.append(fig)

    # FIG 8 - GRAFICO 2 CALL
    fig = px.line(df_call, x='Time', y=[
        "E[N]"], color='Fonte', title='E[N] - Chamada em Tempo Real')
    fig.update_layout(
        xaxis_title='Tempo',
        yaxis_title='E[N]',
        legend_title='Parâmetros'
    )
    figs.append(fig)

    # FIG 9 - GRAFICO 3 CALL
    fig = px.line(df_call, x='Time', y=["E[W]"], color='Fonte', title='E[W] - Chamada em Tempo Real')
    fig.update_layout(
        xaxis_title='Tempo',
        yaxis_title='E[W]',
        legend_title='Parâmetros'
    )
    figs.append(fig)

    # FIG 10 - GRAFICO 5 WEB
    fig = px.line(df_web, x='Time', y=["Lambda", "Mu"], color='Fonte')
    fig.update_layout(
        xaxis_title='Tempo',
        yaxis_title='Taxas',
        legend_title='Parâmetros'
    )
    figs.append(fig)

    # FIG 11 - GRAFICO 6 WEB
    fig = px.line(df_web, x='Time', y="Erro de Little",
                  color='Fonte', title='Erro de Little - Total')
    fig.update_layout(
        xaxis_title='Tempo',
        yaxis_title='Erro',
        legend_title='Parâmetros'
    ) 
    figs.append(fig)

    # FIG 12 - GRAFICO 4 CALL
    fig = px.line(df_call, x='Time', y="Erro de Little",
                  color='Fonte', title='Erro de Little - Chamada em Tempo Real')
    fig.update_layout(
        xaxis_title='Tempo',
        yaxis_title='Erro',
        legend_title='Parâmetros'
    )
    figs.append(fig)

    render_to_html(figs)

    return
