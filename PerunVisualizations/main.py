import pandas as pd
import seaborn as sns
import json
import plotly.express as px
import matplotlib.pyplot as plt
import plotly.graph_objects as go
import numpy as np
import argparse
import dataframe_image as dfi
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
    functions_merge_df['lCPUt'] = functions_merge_df['lCPUt'].astype(float)
    functions_merge_df['eCPUt'] = functions_merge_df['eCPUt'].astype(float)
    functions_merge_df['eWALLt'] = functions_merge_df['eWALLt'].astype(float)
    functions_merge_df['lWALLt'] = functions_merge_df['lWALLt'].astype(float)

    functions_merge_df['cpuTime'] = (functions_merge_df['lCPUt'] - functions_merge_df['eCPUt']) * 1000
    functions_merge_df['wallTime'] = (functions_merge_df['lWALLt'] - functions_merge_df['eWALLt']) * 1000

    functions_merge_df['class'] = functions_merge_df['fName'].str.split('::').str[0]
    functions_merge_df['function'] = functions_merge_df['fName'].str.split('::').str[1]

    return functions_merge_df


def make_objects_df(json_data) -> pd.DataFrame:
    objects_df = pd.DataFrame(json_data['Objects'])
    objects_df = objects_df.dropna()
    objects_df['eWALLt'] = objects_df['eWALLt'].astype(float)
    objects_df['eCPUt'] = objects_df['eCPUt'].astype(float)
    objects_df['objSize'] = objects_df['objSize'].astype(int)
    objects_df['Object type'] = objects_df['objType'].str.split('.').str[0]

    return objects_df


def make_threads_df(df_func, df_objects) -> pd.DataFrame:
    # Group the functions dataframe by TID and count the number of rows
    functions_grouped = df_func.groupby('TID').size().reset_index(name='functions_count')

    # Group the Objects dataframe by TID and count the number of rows
    objects_grouped = df_objects.groupby('TID').size().reset_index(name='objects_count')

    # Merge the two dataframes on TID column
    result_df = pd.merge(functions_grouped, objects_grouped, on='TID')

    # Rename the columns
    result_df.columns = ['thread', 'count_functions', 'count_objects']
    return result_df


def agregate_objectsize_to_functions(df_func, df_objects, thread=None) -> pd.DataFrame:
    df = df_objects.copy()

    df = df[df['fnc'].notnull()]
    df['objSize'] = df['objSize'].astype(int)
    summed = df.groupby('fnc')['objSize'].sum().reset_index()
    count_functions = df.groupby('fnc').size().reset_index(name='count')

    result_df = df_func.copy()
    result_df['objectSizeSum'] = result_df['nOr'].map(summed.set_index('fnc')['objSize'])
    result_df['count'] = result_df['nOr'].map(count_functions.set_index('fnc')['count'])
    result_df['objectSizeSum'].fillna(0, inplace=True)

    if thread is None:
        df_tmp = make_threads_df(df_func, df_objects)
        thread = df_tmp.loc[0, 'thread']

    functions_thread = result_df[result_df['TID'] == thread]

    return functions_thread


def show_treemap(df_func, max_depth):
    stack = np.stack((df_func['class'], df_func['function'], df_func['cpuTime'], df_func['wallTime'], df_func['count']), axis=-1)
    fig = go.Figure(go.Treemap(
        ids=df_func['nOr'],
        labels=df_func['fName'],
        parents=df_func['rFn'],
        marker_colorscale='reds',
        values=df_func['objectSizeSum'],
        hovertemplate='Function: <b>%{customdata[1]}</b>'
                      '<br>Class: <b>%{customdata[0]}</b>'
                      '<br>Allocation size: %{value} Bytes'
                      '<br>Count of allocations: %{customdata[4]}'
                      '<br>WALL time: %{customdata[3]:.3f} ms',
        customdata=stack,
        maxdepth=max_depth
    ))

    # Add a text annotation to the description of the Treemap visualization
    fig.update_layout(
        title="Treemap visualization",
        title_font_size=24,
        title_font_family="Arial",
        title_x=0.5,
        title_y=0.95,
    )
    fig.show()


def show_treemap_func(df_func, max_depth, thread):
    df_func = df_func[df_func['TID'] == thread]
    stack = np.stack((df_func['class'], df_func['function'], df_func['cpuTime'], df_func['wallTime']), axis=-1)
    fig = go.Figure(go.Treemap(
        ids=df_func['nOr'],
        labels=df_func['fName'],
        parents=df_func['rFn'],
        hovertemplate='Function: <b>%{customdata[1]}</b>'
                      '<br>Class: <b>%{customdata[0]}</b>'
                      '<br>WALL time: %{customdata[3]:.3f} ms',
        customdata=stack,
        maxdepth=max_depth
    ))
    # Add a text annotation to the description of the Treemap visualization
    fig.update_layout(
        title="Treemap visualization",
        title_font_size=24,
        title_font_family="Arial",
        title_x=0.5,
        title_y=0.95,
    )
    fig.show()


def filter_objets(df_obj, number) -> pd.DataFrame:
    df2 = df_obj.copy()
    maxValue = max(df2['eWALLt'])
    df2['eWALLt'] = maxValue - df2['eWALLt']
    grouped = df2.groupby('objType')['objSize'].sum().reset_index()
    grouped = grouped.sort_values('objSize', ascending=False)

    df_topN = grouped.nlargest(number, 'objSize')
    df2 = df2[df2['objType'].isin(df_topN['objType'])]
    df2 = df2.sort_values('objSize', ascending=False).reset_index()

    return df2


def make_scatterplot(df_obj, name):
    df_obj = df_obj[df_obj['objType'] != ""]
    df_obj = df_obj.rename(columns={'objSize': 'Object size (Bytes)'})

    plt.figure(figsize=(14, 8))
    ax = sns.scatterplot(x='eCPUt', y='objType', size='Object size (Bytes)', hue='Object type', data=df_obj)

    ax.set_ylabel('Object types')
    ax.set_xlabel('Wall time')
    plt.subplots_adjust(left=0.3)
    plt.title('Scatter Plot of Object Allocation ')

    # Show the plot
    plt.savefig("Plots/"+name)


def make_scatterplot_with_func(df_func, df_obj, name):
    df_obj = df_obj[df_obj['objType'] != ""]
    df_obj = df_obj.rename(columns={'objSize': 'Object size (Bytes)'})
    plt.figure(figsize=(14, 8))
    ax = sns.scatterplot(x='eWALLt', y='fName', size='Object size (Bytes)', hue='Object type', data=df_obj)

    ax.set_ylabel('Functions')
    ax.set_xlabel('Wall time')
    plt.subplots_adjust(left=0.3)
    plt.title('Scatter Plot of Object Allocation ')

    # Show the plot
    plt.savefig("Plots/"+name)


def display_top_callbacks_functions(df_func, topN, name, sortBy='count'):
    by = 'fID'
    if sortBy == 'count':
        by = 'fName'
    count_functions = df_func.groupby(by).size().reset_index(name='count')
    grouped_df = df_func.groupby([by]).agg({
        'class': 'first',
        'function': 'first',
        'wallTime': 'sum'
    }).reset_index()
    df = pd.merge(grouped_df, count_functions, on=by)
    df = df.sort_values(sortBy, ascending=False).reset_index()
    df = df.nlargest(topN, sortBy)
    df = df.drop('index', axis=1)
    if by == 'fID':
        df = df.drop('fID', axis=1)
    df = df.rename(columns={'fName': 'Function name'})
    df['wallTime'] = df['wallTime'].round(3).astype(str)
    df['wallTime'] = df['wallTime'] + ' ms'
    df = df.rename(columns={'wallTime': 'Time'})

    dfi.export(df, 'Experiments/CacheManagerExample/tableFunctions'+name+sortBy+'.png', table_conversion="png")


def display_top_allocation_objects(df_obj, topN, name, sortBy='count'):
    df_obj = df_obj[df_obj['objType'] != ""]
    count_functions = df_obj.groupby('objType').size().reset_index(name='count')
    grouped_df = df_obj.groupby(['objType']).agg({
        'objSize': 'sum',
    }).reset_index()
    df = pd.merge(grouped_df, count_functions, on='objType')
    df = df.sort_values(sortBy, ascending=False).reset_index()
    df = df.nlargest(topN, sortBy)
    df = df.drop('index', axis=1)
    df['objSize'] = df['objSize'].astype(str)
    df['objSize'] = df['objSize'] + ' B'
    df = df.rename(columns={'objSize': 'Object size (Bytes)'})

    dfi.export(df, 'Experiments/CacheManagerExample/tableObjects'+name+sortBy+'.png', table_conversion="png")


if __name__ == "__main__":
    name = 'PerunCSharpProfiler_2023-05-01_15_37_27'
    data = parse_json('Experiments/CacheManagerExample/run2/'+name+'.json')
    mode = 1

    if mode == 0:
        dfFunctions = make_function_df(data)
        dfObjects = make_objects_df(data)
        tmp_df = make_threads_df(dfFunctions, dfObjects)
        display(tmp_df)
        display_top_callbacks_functions(dfFunctions, 10, name, 'count')
        display_top_callbacks_functions(dfFunctions, 10, name, 'wallTime')
        display_top_allocation_objects(dfObjects, 10, name, 'count')
        display_top_allocation_objects(dfObjects, 10, name, 'objSize')

        agregate_df_functions = agregate_objectsize_to_functions(dfFunctions, dfObjects)
        show_treemap(agregate_df_functions, 5)
        df_obj_filter = filter_objets(dfObjects, 10)
        make_scatterplot(df_obj_filter, name)

    elif mode == 1:
        dfFunctions = make_function_df(data)
        show_treemap_func(dfFunctions, 7, '0x000002C758E8C0A0')
        display_top_callbacks_functions(dfFunctions, 10, name, 'count')
        display_top_callbacks_functions(dfFunctions, 10, name, 'wallTime')

    elif mode == 2:
        dfObjects = make_objects_df(data)
        display_top_allocation_objects(dfObjects, 10, name, 'count')
        display_top_allocation_objects(dfObjects, 10, name, 'objSize')
        df_obj_filter = filter_objets(dfObjects, 10)
        make_scatterplot(df_obj_filter, name)

