import pandas as pd
import seaborn as sns
import json
import matplotlib.pyplot as plt
import plotly.graph_objects as go
import numpy as np
import argparse
import dataframe_image as dfi
from IPython.display import display
import os


def parse_json(file):
    """
    Function to parse JSON file
    :param file: profile JSON data
    :return: data
    """
    with open(file) as f:
        return json.load(f)


def make_function_df(json_data) -> pd.DataFrame:
    """
    Function to make DataFrame for functions
    :param json_data: profile data
    :return: DataFrame for functions
    """
    functions_df = pd.DataFrame(json_data['functions'])

    function_names_df = pd.DataFrame.from_dict(json_data['functionNames'], orient='index', columns=['fName'])
    function_names_df.index.name = 'fID'
    functions_merge_df = pd.merge(functions_df, function_names_df, on='fID')
    functions_merge_df = functions_merge_df.dropna()

    functions_merge_df['lCPUt'] = functions_merge_df['lCPUt'].astype(float)
    functions_merge_df['eCPUt'] = functions_merge_df['eCPUt'].astype(float)
    functions_merge_df['eWALLt'] = functions_merge_df['eWALLt'].astype(float)
    functions_merge_df['lWALLt'] = functions_merge_df['lWALLt'].astype(float)

    # make column for time in ms
    functions_merge_df['cpuTime'] = (functions_merge_df['lCPUt'] - functions_merge_df['eCPUt']) * 1000
    functions_merge_df['wallTime'] = (functions_merge_df['lWALLt'] - functions_merge_df['eWALLt']) * 1000

    functions_merge_df['class'] = functions_merge_df['fName'].str.split('::').str[0]
    functions_merge_df['function'] = functions_merge_df['fName'].str.split('::').str[1]
    functions_merge_df['depth'] = functions_merge_df['depth'].astype(int)

    return functions_merge_df


def make_objects_df(json_data) -> pd.DataFrame:
    """
    Function to make DataFrame for objects
    :param json_data: profile data
    :return: DataFrame for objects
    """
    objects_df = pd.DataFrame(json_data['Objects'])
    objects_df = objects_df.dropna()
    objects_df['eWALLt'] = objects_df['eWALLt'].astype(float)
    objects_df['eCPUt'] = objects_df['eCPUt'].astype(float)
    objects_df['objSize'] = objects_df['objSize'].astype(int)
    objects_df['Object type'] = objects_df['objType'].str.split('.').str[0]
    objects_df['untilGC'] = objects_df['untilGC'].astype(int)
    objects_df['GC'] = objects_df['GC'].astype(int)

    return objects_df


def make_threads_df_func(df_func) -> pd.DataFrame:
    """
    Function to make DataFrame for threads with count of call functions
    :param df_func: DataFrame of functions
    :return: DataFrame of threads
    """
    functions_grouped = df_func.groupby('TID').size().reset_index(name='functions_count')
    return functions_grouped


def make_threads_df_obj(df_objects) -> pd.DataFrame:
    """
    Function to make DataFrame for threads with count of object allocations
    :param df_objects: DataFrame of objects
    :return: DataFrame of threads
    """
    objects_grouped = df_objects.groupby('TID').size().reset_index(name='objects_count')
    return objects_grouped


def agregate_objectsize_to_functions(df_func, df_objects, thread=None) -> pd.DataFrame:
    """
    Function to make dataframe of functions with size of allocated objects
    :param df_func: DataFrame of functions
    :param df_objects: DataFrame of objects
    :param thread: Thread name
    :return: DataFrame of functions with object size
    """
    df = df_objects.copy()

    df = df[df['fnc'].notnull()]
    df['objSize'] = df['objSize'].astype(int)
    summed = df.groupby('fnc')['objSize'].sum().reset_index()
    count_functions = df.groupby('fnc').size().reset_index(name='count')

    result_df = df_func.copy()
    result_df['objectSizeSum'] = result_df['nOr'].map(summed.set_index('fnc')['objSize'])
    result_df['count'] = result_df['nOr'].map(count_functions.set_index('fnc')['count'])
    result_df['objectSizeSum'].fillna(0, inplace=True)
    result_df['count'].fillna(0, inplace=True)

    if thread is None:
        df_tmp = make_threads_df_func(df_func)
        thread = df_tmp.loc[0, 'thread']

    functions_thread = result_df[result_df['TID'] == thread]

    return functions_thread


def show_treemap(df_func, max_depth):
    """
    Function to make Treemap graph with size of object allocations in function
    :param df_func: DataFrame of functions
    :param max_depth: max depth
    """
    df_func = df_func[df_func['depth'] <= max_depth]
    stack = np.stack((df_func['class'], df_func['function'], df_func['cpuTime'], df_func['wallTime'], df_func['count']),
                     axis=-1)
    fig = go.Figure(go.Treemap(
        ids=df_func['nOr'],
        parents=df_func['rFn'],
        labels=df_func['fName'],
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
    """
    Function to make Treemap graph of functions
    :param df_func: DataFrame of functions
    :param max_depth: max depth
    :param thread: thread name
    """
    df_func = df_func[df_func['TID'] == thread]
    df_func = df_func[df_func['depth'] <= max_depth]
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
    """
    Function to filter objects for Scatter plot graph
    :param df_obj: DataFrame of objects
    :param number: number of objects to filter
    :return: DataFrame of filtered objects
    """
    df2 = df_obj.copy()
    maxValue = max(df2['eWALLt'])
    df2['eWALLt'] = maxValue - df2['eWALLt']
    grouped = df2.groupby('objType')['objSize'].sum().reset_index()
    grouped = grouped.sort_values('objSize', ascending=False)

    df_topN = grouped.nlargest(number, 'objSize')
    df2 = df2[df2['objType'].isin(df_topN['objType'])]
    df2 = df2.sort_values('objSize', ascending=False).reset_index()

    return df2


def make_scatterplot(df_obj, name, valueX, save_path):
    """
    Function to show scatter plot
    :param df_obj: DataFrame of objects
    :param name: name of save svg
    :param valueX: value of x cpu/wall time
    :param save_path: path to save plot
    """
    df_obj = df_obj[df_obj['objType'] != ""]
    df_obj = df_obj.rename(columns={'objSize': 'Object size (Bytes)'})

    if valueX == "wallTime":
        valueX = "eWALLt"
    if valueX == "cpuTime":
        valueX = "eCPUt"

    plt.figure(figsize=(14, 8))
    ax = sns.scatterplot(x=valueX, y='objType', size='Object size (Bytes)', hue='Object type', data=df_obj)

    ax.set_ylabel('Object types')
    ax.set_xlabel('WALL time [s]')
    plt.subplots_adjust(left=0.3)
    plt.title('Scatter Plot of Object Allocation ')

    # Show the plot
    plt.savefig(save_path + "/scatterPlot" + name)


def display_top_callbacks_functions(df_func, topN, name, save_path, sortBy):
    """
    Function to make plot of top callbacks functions
    :param df_func: DataFrame of functions
    :param topN: number of functions to show
    :param name: name of save plot
    :param save_path: path to save plot
    :param sortBy: sort by
    """
    by = 'fID'
    if sortBy == 'count':
        by = 'fName'
    count_functions = df_func.groupby(by).size().reset_index(name='count')
    grouped_df = df_func.groupby([by]).agg({
        'class': 'first',
        'function': 'first',
        'wallTime': 'sum',
        'cpuTime': 'sum'
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

    dfi.export(df, save_path+'/tableFunctions' + name + sortBy + '.png', table_conversion="png")


def display_top_allocation_objects(df_obj, topN, name, save_path, sortBy):
    """
    Function to display top allocations of objects
    :param df_obj: DataFrame of objects
    :param topN: number of objects to show
    :param name: name of save plot
    :param save_path: path to save plot
    :param sortBy: sort by
    """
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

    dfi.export(df, save_path +'/tableObjects' + name + sortBy + '.png', table_conversion="png")


def make_gc_df(data, name, time) -> pd.DataFrame:
    """
    Function to make garbage collection dataFrame
    :param data: profile JSON data
    :param name: name of save plot
    :param time: GC pressure calculation
    :return: DataFrame of garbage collection data
    """
    gc_events = []
    for key in data:
        if key.startswith('GCStarted'):
            gc_num = int(key[9:])
            gc_event = {
                'GC': gc_num,
                'Type': 'Start',
                'TID': data[key]['TID'],
                'Gen0': data[key]['Gen0'],
                'Gen1': data[key]['Gen1'],
                'Gen2': data[key]['Gen2'],
                'eWALLt': data[key]['eWALLt'],
                'eCPUt': data[key]['eCPUt'],
                'lWALLt': None,
                'lCPUt': None
            }
            gc_events.append(gc_event)
        elif key.startswith('GCFinished'):
            gc_num = int(key[10:])
            gc_event = next((event for event in gc_events if event['GC'] == gc_num), None)
            if gc_event:
                gc_event.update({
                    'Type': 'Finish',
                    'lWALLt': data[key]['lWALLt'],
                    'lCPUt': data[key]['lCPUt']
                })
    df = pd.DataFrame(gc_events)

    df['eWALLt'] = df['eWALLt'].astype(float)
    df['lWALLt'] = df['lWALLt'].astype(float)

    df['Time [ms]'] = (df['lWALLt'] - df['eWALLt']) * 1000
    df['Percent Time'] = df['Time [ms]'] / time * 100
    print(df['Percent Time'].sum())
    dfi.export(df, 'Experiments/tableGC' + name + '.png', table_conversion="png")

    return df


def make_profiler_df(data) -> pd.DataFrame:
    """
    Function to make DataFrame of program start/finish
    :param data: data JSON profile
    :return: DataFrame of program start/finish
    """
    pr_events = []
    for key in data:
        if key.startswith('Profiler'):
            pr_event = {
                'act': data[key]['act'],
                'PID': data[key]['PID'],
                'TID': data[key]['TID'],
                'eWALLt': data[key]['eWALLt'],
                'eCPUt': data[key]['eCPUt'],
            }
            pr_events.append(pr_event)
    df = pd.DataFrame(pr_events)
    df['eWALLt'] = df['eWALLt'].astype(float)
    return df


def start_program():
    # Parse arguments
    parser = argparse.ArgumentParser(description="PerunCSharp Profiler visualization module")
    parser.add_argument("-p", "--path", default="Data/data.json", help="Set path to profile data file (extension .json)")
    parser.add_argument("-gs", "--gScatterPlot", choices=['wallTime', 'cpuTime'],
                        help="Scatter plot in wall/cpu time, second value is number of types of objets")
    parser.add_argument("-gt", "--gTreemap", choices=['functions', 'functionsWithAlloc'],
                        help="Treemap mode for only functions or functions with allocation (default functions)")
    parser.add_argument("-t", "--thread", help="Set name of thread to be shown at treeMap graph or tables")
    parser.add_argument("-n", "--number", default=10, type=int, help="show this number, can be use with any graph/table"
                                                                     "(default 10)")
    parser.add_argument("-s", "--savePath", default="Plots", help="Set path to save plot (default /Plots)")
    parser.add_argument("-m", "--mode", choices=[0, 1, 2], type=int, default=0, help="Set mode for profile data"
                                                                                     "(default 0)")
    parser.add_argument("-tabF", "--tableFunctions", choices=['count', 'wallTime', 'cpuTime'],
                        help="Make table plot for functions")
    parser.add_argument("-tabO", "--tableObjects", choices=['count', 'objSize'],
                        help="Make table plot for objects")
    parser.add_argument("-tabT", "--tableThreads", choices=['functions', 'objects'],
                        help="Make plot table of threads with count of functions or objects")

    args = parser.parse_args()
    name = os.path.splitext(os.path.basename(args.path))[0]

    try:
        data = parse_json(args.path)
    except:
        print("Error:loading JSON file failed!")
        return
    mode = args.mode
    thread = args.thread

    # mode configuration
    if mode == 0:
        try:
            dfFunctions = make_function_df(data)
            dfObjects = make_objects_df(data)
            if thread is None:
                thread_df = make_threads_df_func(dfFunctions)
                thread = thread_df.loc[thread_df['functions_count'].idxmax(), 'TID']
        except:
            print("Error: profile data are not compatible!")
            return
    elif mode == 1:
        if args.gScatterPlot is not None or args.tableObjects is not None:
            print("Error: this graph is not compatible with this mode!")
            return
        try:
            dfFunctions = make_function_df(data)
            if thread is None:
                thread_df = make_threads_df_func(dfFunctions)
                thread = thread_df.loc[thread_df['functions_count'].idxmax(), 'TID']
        except:
            print("Error: profile data are not compatible!")
            return
    elif mode == 2:
        if args.gTreemap is not None or args.tableFunctions is not None:
            print("Error: this visualization is not compatible!")
            return
        try:
            dfObjects = make_objects_df(data)
            if thread is None:
                thread_df = make_threads_df_obj(dfObjects)
                thread = thread_df.loc[thread_df['objects_count'].idxmax(), 'TID']
        except:
            print("Error: profile data are not compatible!")
            return
    else:
        print("Error: this mode is not supported")
        return

    if getattr(args, 'gTreemap'):
        if args.gTreemap == "functionsWithAlloc":
            agregate_df = agregate_objectsize_to_functions(dfFunctions, dfObjects, thread)
            show_treemap(agregate_df, args.number)
        elif args.gTreemap == "functions":
            show_treemap_func(dfFunctions, args.number, thread)

    if getattr(args, 'gScatterPlot'):
        df_obj_filter = filter_objets(dfObjects, args.number)
        make_scatterplot(df_obj_filter, name, args.gScatterPlot, args.savePath)

    if getattr(args, 'tableFunctions'):
        display_top_callbacks_functions(dfFunctions, args.number, name, args.savePath, args.tableFunctions)

    if getattr(args, 'tableObjects'):
        display_top_allocation_objects(dfObjects, args.number, name, args.savePath,  args.tableObjects)

    if getattr(args, 'tableThreads'):
        if mode != 2 and args.tableThreads == "functions":
            df_thread_fn = make_threads_df_func(dfFunctions)
            display(df_thread_fn)
        elif mode != 1 and args.tableThreads == "objects":
            df_thread_ob = make_threads_df_obj(dfObjects)
            display(df_thread_ob)
        else:
            print("Error: profile data are not compatible!")


if __name__ == "__main__":
    start_program()
