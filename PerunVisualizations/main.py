import pandas as pd
import seaborn as sns
import json
import plotly.express as px
import matplotlib.pyplot as plt
import plotly.graph_objects as go
import numpy as np
from IPython.display import display


def parse_json(file):
    with open(file) as f:
        return json.load(f)


def make_function_df(json_data) -> pd.DataFrame:
    functions_df = pd.DataFrame(json_data['functions'])

    function_names_df = pd.DataFrame.from_dict(json_data['functionNames'], orient='index', columns=['fName'])
    function_names_df.index.name = 'fID'
    functions_merge_df = pd.merge(functions_df, function_names_df, on='fID')
    functions_merge_df = functions_merge_df.dropna()

    return functions_merge_df


def make_objects_df(json_data) -> pd.DataFrame:
    objects_df = pd.DataFrame(json_data['Objects'])
    objects_df = objects_df.dropna()
    objects_df['eWALLt'] = objects_df['eWALLt'].astype(float)
    objects_df['objSize'] = objects_df['objSize'].astype(int)

    return objects_df


def display_threads(df_func, df_objects):
    df_tmp = df_func.copy()
    df_tmp = df_tmp.groupby('TID').count().reset_index()

    print(df_tmp)


def agregate_objectsize_to_functions(df_func, df_objects, thread) -> pd.DataFrame:
    df = df_objects.copy()

    df = df[df['fnc'].notnull()]
    df['objSize'] = df['objSize'].astype(int)
    summed = df.groupby('fnc')['objSize'].sum().reset_index()

    functionsInThread = df_func.copy()
    functionsInThread['objectSizeSum'] = functionsInThread['nOr'].map(summed.set_index('fnc')['objSize'])
    functionsInThread['objectSizeSum'].fillna(0, inplace=True)

    functionsInThread = functionsInThread[functionsInThread['TID'] == thread]

    return functionsInThread


def show_treemap(df_func, max_depth):
    fig = go.Figure(go.Treemap(
        ids=df_func['nOr'],
        labels=df_func['fName'],
        parents=df_func['rFn'],
        marker_colorscale='reds',
        values=df_func['objectSizeSum'],
        textinfo="label+value+percent parent+percent entry+percent root",
        maxdepth=10
    ))
    fig.show()


if __name__ == "__main__":
    data = parse_json('Data\PerunCSharpProfiler_2023-04-29_2141.json')
    dfFunctions = make_function_df(data)
    dfObjects = make_objects_df(data)

    display_threads(dfFunctions, dfObjects)
    agregate_df_functions = agregate_objectsize_to_functions(dfFunctions, dfObjects, '1674978400')
    #agregate_df_functions = agregate_df_functions.groupby('fID').count().reset_index()
    #print(agregate_df_functions['fName'])

    show_treemap(agregate_df_functions, 10)
