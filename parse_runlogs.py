#!/usr/bin/env python3

import os
import sys
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from graphviz import Digraph
import csv
import shutil

datadir = sys.argv[1]

data_points = []
headers = []
myLog = []
lambda_list = []
thunk_list = []

'''
    lambda类设计
    id, time ,parent lambda, children lambda, get thunk, upload thunk, read num, read size, write num, write size
    thunk class design
    hash value, lifetime
'''
class Thunk:
    def __init__(self) -> None:
        self.hash_value = ''
        self.write_time_start = []
        self.write_time_end = []
        self.wrote_time = []
        self.read_time_start = []
        self.read_time_end = []
        self.read_time = []
        self.lifetime = 0
        self.size = 0
        self.read_lambda = []
        self.write_lambda = []
    def compute_lifetime(self):
        self.write_time_end.sort()
        self.read_time_start.sort()
        # thunk life time equals to the last read time - the first write time
        if self.hash_value == 'V5lALPzBnjYgbF4ZAG.xmDvjWDLCYvvazhD4K45UmzcE00083f1b':
            print('write start ',self.write_time_start)
            print('write end ',self.write_time_end)
            print('read start ',self.read_time_start)
            print('read end ',self.read_time_end)
        if len(self.write_time_start) == 0:
            self.lifetime = self.read_time_end[-1]
        elif len(self.read_time_start) != 0 and len(self.write_time_start) != 0:
            self.lifetime = self.read_time_end[-1] - self.write_time_end[0]


class Lambda:
    def __init__(self) -> None:
        self.hash = ''
        self.id_ = 0
        self.read_thunk_time = 0
        self.do_clean_time = 0
        self.get_dependencies_time = 0
        self.execute_time = 0
        self.upload_time = 0
        self.read_start = 0
        self.read_end = 0
        self.read_num = 0
        self.read_size = 0
        self.write_start = 0
        self.write_end = 0
        self.write_num = 0
        self.write_size = 0
        self.execute = ''
        self.read_hash = []
        self.write_hash = []
        self.parent_lambda = []
        self.children_lambda = []

def check_in_hashList(hash_value):
    global thunk_list
    for thunk in thunk_list:
        if hash_value == thunk.hash_value:
            return True, thunk
    return False, None

def compute_lambda_read_time(lambda_entity):
    time_start = lambda_entity.read_start
    time_end = lambda_entity.read_end
    read_time = time_end - time_start
    return read_time

def compute_lambda_write_time(lmbd):
    time_start = lmbd.write_start
    time_end = lmbd.write_end
    write_time = time_end - time_start
    return write_time

def generate_thunk():
    global lambda_list
    global thunk_list
    for lmbd in lambda_list:
        for h in lmbd.read_hash:
            t , thunk = check_in_hashList(h[0])
            if not t:
                thunk = Thunk()
                thunk.hash_value = h[0]
                thunk.size = h[1]
                thunk.read_time_start.append(lmbd.read_start)
                thunk.read_time_end.append(lmbd.read_end)
                thunk.read_time.append(compute_lambda_read_time(lmbd))
                thunk.read_lambda.append(lmbd)
                thunk_list.append(thunk)
            else:
                thunk.read_time_start.append(lmbd.read_start)
                thunk.read_time_end.append(lmbd.read_end)
                thunk.read_time.append(compute_lambda_read_time(lmbd))
                thunk.read_lambda.append(lmbd)
        for h in lmbd.write_hash:
            t , thunk = check_in_hashList(h[0])
            if not t:
                thunk = Thunk()
                thunk.hash_value = h[0]
                thunk.size = h[1]
                thunk.write_time_start.append(lmbd.write_start)
                thunk.write_time_end.append(lmbd.write_end)
                thunk.wrote_time.append(compute_lambda_write_time(lmbd))
                thunk.write_lambda.append(lmbd)
                thunk_list.append(thunk)
            else:
                thunk.write_time_start.append(lmbd.write_start)
                thunk.write_time_end.append(lmbd.write_end)
                thunk.wrote_time.append(compute_lambda_write_time(lmbd))
                thunk.write_lambda.append(lmbd)
    for thunk in thunk_list:
        thunk.compute_lifetime()


def create_lambda(timelog):
    lmbd = Lambda()
    for log in timelog:
        if log[0] == 'started':
            lmbd.id_ = log[1]
        elif log[0] == 'hash':
            lmbd.hash = log[1]
        elif log[0] == 'read_thunk':
            lmbd.read_thunk_time = log[1]
        elif log[0] == 'do_cleanup':
            lmbd.do_clean_time = log[1]
        elif log[0] == 'get_dependencies':
            lmbd.get_dependencies_time = log[1]
        elif log[0] == 'execute':
            lmbd.execute_time = log[1]
        elif log[0] == 'command':
            lmbd.execute = log[1]
        elif log[0] == 'upload_output':
            lmbd.upload_time = log[1]
        elif log[0] == 'get_dependencies_num':
            lmbd.read_num = log[1]
        elif log[0] == 'get_dependencies_size':
            lmbd.read_size = log[1]
        elif log[0] == 'upload_output_num':
            lmbd.write_num = log[1]
        elif log[0] == 'upload_output_size':
            lmbd.write_size = log[1]
        elif log[0] == 'get_hash':
            lmbd.read_hash.append([log[1], log[2]])
            # lmbd.get_thunk.append(log[1])
        elif log[0] == 'upload_hash':
            lmbd.write_hash.append([log[1], log[2]])
            # lmbd.upload_thunk.append(log[1])
        elif log[0] == 'get_dependencies_start_time':
            lmbd.read_start = log[1]       
        elif log[0] == 'get_dependencies_end_time':
            lmbd.read_end = log[1]
        elif log[0] == 'upload_output_start_time':
            lmbd.write_start = log[1]
        elif log[0] == 'upload_output_end_time':
            lmbd.write_end = log[1]
    global lambda_list
    lambda_list.append(lmbd)
         
'''
    Generate the dependency graphy, DAG
'''
# classify_dependency() loops through lambda_list and calls classify_write_read for each lambda in lambda_list 
# in order to classify dependencies between the lambdas. 
# classify_write_read() then checks if there are any hashes in write_hash that are also in read_hash. 
# If there is a match, then the child_lambda and parent_lambda lists are updated to reflect the dependency.
def classify_dependency():
    global lambda_list
    for current_lambda in lambda_list:
        for lambda_x in lambda_list:
            if(current_lambda != lambda_x):
                classify_write_read(current_lambda, lambda_x)

def classify_write_read(current_lambda, lambda_x):
    for hash_v in current_lambda.write_hash:
        if hash_v in lambda_x.read_hash:
            current_lambda.children_lambda.append(lambda_x)
            lambda_x.parent_lambda.append(current_lambda)

# def classify_dependency():
#     global lambda_list
#     for current_lambda in lambda_list:
#         for lambda_x in lambda_list:
#             if(current_lambda != lambda_x):
#                 for hash_v in current_lambda.write_hash:
#                     if hash_v in lambda_x.read_hash:
#                         current_lambda.children_lambda.append(lambda_x)
#                         lambda_x.parent_lambda.append(current_lambda)

def traverse_create(g, parent, child):
    label = format_label(child)
    g.node(child.hash, label)
    g.edge(parent.hash, child.hash)
    if len(child.children_lambda) != 0:
        for chd in child.children_lambda:
            traverse_create(g, child, chd)

def format_label(node) -> str:
    lb_id = 'id: ' + node.hash + '\n'
    # if the command is too long, then truncate it to 100 characters
    if len(node.execute) > 100:
        node.execute = node.execute[:100]
    label = lb_id + 'command: ' + node.execute + '\n'
    label += 'read num: ' + str(node.read_num) + '\n'
    label += 'read size: ' + str(node.read_size) + ' bytes' + '\n'
    label += 'write num: ' + str(node.write_num) + '\n'
    label += 'write size: ' + str(node.write_size) + ' bytes'
    return label

def plot_dependency_graph():
    # plot the dependency graph of the lambda function
    global lambda_list
    root_lambda = []
    g = Digraph("Dependency",strict=True)
    # find the root lambda
    for lmbd in lambda_list:
        if len(lmbd.parent_lambda) == 0:
            root_lambda.append(lmbd)
    # add root lambda node
    for node in root_lambda:
        label = format_label(node)
        g.node(node.hash,label=label)
    # add child lambda node
    for node in root_lambda:
        if len(node.children_lambda) != 0:
            for chd_node in node.children_lambda:
                traverse_create(g, node, chd_node)
    # render the graph
    g.render(datadir.split('/')[5]+'_dep.gv', directory="/home/handsonhuang/gg/temp/",format='png')
    g.render(datadir.split('/')[5]+'_dep.gv', directory="/home/handsonhuang/gg/temp/",format='svg')

# traverse the dependency graph without command label
def traverse_create_without_command(g, parent, child):
    label = format_label_without_command(child)
    g.node(child.hash, label)
    g.edge(parent.hash, child.hash)
    if len(child.children_lambda) != 0:
        for chd in child.children_lambda:
            traverse_create_without_command(g, child, chd)

# format the label without command
def format_label_without_command(node) -> str:
    """Create a string to be used as node label, based on the node's
    statistics."""
    # First, create a list of the statistics we want to display.
    # Each list item is a string that will be concatenated to the final
    # label.
    stats = [
        'id: ' + node.hash+'\n',
        'read num: ' + str(node.read_num)+'\n',
        'read size: ' + str(node.read_size) + ' bytes'+ '\n',
        'write num: ' + str(node.write_num)+'\n',
        'write size: ' + str(node.write_size) + ' bytes'
    ]
    # Join the list items with newlines.
    label = ''.join(stats)
    return label

#  plot the dependency graph without command label
def plot_dependency_graph_without_command():
    global lambda_list
    root_lambda = []
    g = Digraph("Dependency",strict=True)
    for lmbd in lambda_list:
        if len(lmbd.parent_lambda) == 0:
            root_lambda.append(lmbd)
    for node in root_lambda:
        label = format_label_without_command(node)
        g.node(node.hash, label=label)
    for node in root_lambda:
        if len(node.children_lambda) != 0:
            for chd_node in node.children_lambda:
                traverse_create_without_command(g, node, chd_node)
    g.render(datadir.split('/')[5]+'_dep_without_command.gv', directory="/home/handsonhuang/gg/temp/",format='png')
    g.render(datadir.split('/')[5]+'_dep_without_command.gv', directory="/home/handsonhuang/gg/temp/",format='svg')


'''
    Generate the CDF
'''
def plot_cdf(data_log):
    for (name, data) in data_log.items():
        denominator = len(data)
        Data = pd.Series(data)
        Fre=Data.value_counts()
        Fre_sort=Fre.sort_index(axis=0,ascending=True)
        Fre_df=Fre_sort.reset_index()
        Fre_df[0]=Fre_df[0]/denominator
        Fre_df.columns=[name,'Fre']
        Fre_df['cumsum']=np.cumsum(Fre_df['Fre'])
        Fre_df.to_excel('/home/handsonhuang/gg/temp/'+name+'.xlsx')
        # print(Fre_df)

        #创建画布
        plot=plt.figure()
        #只有一张图，也可以多张
        ax1=plot.add_subplot(1,1,1)
        #按照Rds列为横坐标，累计概率分布为纵坐标作图
        ax1.plot(Fre_df[name],Fre_df['cumsum'])
        #图的标题
        ax1.set_title("CDF")
        #横轴名
        ax1.set_xlabel(name)
        #纵轴名
        ax1.set_ylabel("P")
        #横轴的界限
        ax1.set_xlim(min(data),max(data))
        path = r'/home/handsonhuang/gg/temp/'+datadir.split('/')[5]+'_'+name
        plt.savefig(path)

def create_cdf(timelog):
    global thunk_list
    data_log = {'read_num':[], 'write_num':[], 'read_size':[], 'write_size':[], 'thunk_size':[]}
    for log in timelog:
        if log[0] == 'get_dependencies_num':
            # read_num.append(log[1])
            data_log['read_num'].append(log[1])
        elif log[0] == 'upload_output_num':
            # write_num.append(log[1])
            data_log['write_num'].append(log[1])
        elif log[0] == 'get_dependencies_size':
            # read_size.append(log[1])
            data_log['read_size'].append(log[1])
        elif log[0] == 'upload_output_size':
            # write_size.append(log[1])
            data_log['write_size'].append(log[1])
    for thunk in thunk_list:
        data_log['thunk_size'].append(thunk.size)
    plot_cdf(data_log)


''''
    Create the csv file about thunk's read and wrote lambdas and its lifetime

    thunk
    hash_value    read_lambda    write_lambda    life_time
'''
def create_thunk_csv():
    global thunk_list
    headers_read = ['hash', 'hash_size', 'read_lambda']
    headers_write = ['hash', 'hash_size', 'write_lambda']
    headers_life = ['hash', 'read_num', 'write_num','lifetime']
    # create the hash-read
    with open('/home/handsonhuang/gg/temp/hash_read.csv', 'w', encoding='utf8', newline='') as f:
        writer = csv.writer(f)
        rows = []
        for thunk in thunk_list:
            if len(thunk.read_lambda) != 0 and len(thunk.write_lambda) != 0:
                row = []
                row.append(thunk.hash_value)
                row.append(thunk.size)
                for lmbda in thunk.read_lambda:
                    row.append(str(lmbda.hash))
                rows.append(tuple(row))
            # else:
            #     row = []
            #     row.append(thunk.hash_value)
            #     row.append('null')
            #     rows.append(tuple(row))
        writer.writerow(headers_read)
        writer.writerows(rows)

    with open('/home/handsonhuang/gg/temp/hash_write.csv', 'w', encoding='utf8', newline='') as f:
        writer = csv.writer(f)
        rows = []
        for thunk in thunk_list:
            if len(thunk.write_lambda) != 0 and len(thunk.read_lambda) != 0:
                for lmbda in thunk.write_lambda:
                    row = []
                    row.append(thunk.hash_value)
                    row.append(thunk.size)
                    row.append(str(lmbda.hash))
                    rows.append(tuple(row))
            # else:
            #     row = []
            #     row.append(thunk.hash_value)
            #     row.append('null')
            #     rows.append(tuple(row))
        writer.writerow(headers_write)
        writer.writerows(rows)

    with open('/home/handsonhuang/gg/temp/hash_lifetime.csv', 'w', encoding='utf8', newline='') as f:
        writer = csv.writer(f)
        rows = []
        for thunk in thunk_list:
            if len(thunk.write_lambda) != 0 and len(thunk.read_lambda) != 0:
                row = []
                row.append(thunk.hash_value)
                row.append(len(thunk.read_lambda))
                row.append(len(thunk.write_lambda))
                row.append(thunk.lifetime)
                rows.append(tuple(row))
        writer.writerow(headers_life)
        writer.writerows(rows)

'''
    create the runtime csv
    
    read_thunk_time
    do_clean_time
    get_dependencies_time
    execute_time
    upload_time

'''
def create_runtime_csv():
    global lambda_list
    headers = ['id', 'read_thunk', 'do_clean', 'get_dependencies', 'execute', 'upload_output', 'total_time']
    with open('/home/handsonhuang/gg/temp/task_runtime.csv', 'w', encoding='utf8', newline='') as f:
        writer = csv.writer(f)
        rows = []
        for lmbd in lambda_list:
            total = lmbd.read_thunk_time + lmbd.do_clean_time + lmbd.get_dependencies_time + lmbd.execute_time + lmbd.upload_time
            row = []
            row.append(lmbd.hash)
            row.append(lmbd.read_thunk_time)
            row.append(lmbd.do_clean_time)
            row.append(lmbd.get_dependencies_time)
            row.append(lmbd.execute_time)
            row.append(lmbd.upload_time)
            row.append(total)
            rows.append(tuple(row))
        writer.writerow(headers)
        writer.writerows(rows)


'''
    Parse the logfile in given path
'''
for logfile in os.listdir(datadir):
    path = os.path.join(datadir, logfile)

    with open(path, "r") as log:
        timelog = []

        for line in log:
            if not timelog:
                timelog += [('hash', (logfile))]
                timelog += [('started', int(line))] # add the start time and identifier the log
            else:
                if line.split()[1] != 'Numbers' and line.split()[1] != 'Sizes' and line.split()[1] != 'Hash':
                    # print(line.split())
                    if line.split()[0] == 'get_hash' or line.split()[0] == 'upload_hash':
                        timelog += [(line.split()[0], (line.split()[1]), int(line.split()[2]))]
                    elif line.split()[0] == 'Execute':
                        timelog += [('command', line.split()[2])]
                    else:
                        timelog += [(line.split()[0], int(line.split()[1]))]
        create_lambda(timelog)
        myLog += timelog
        point = [x[1] for x in timelog]
        data_points += [point]

        if not headers:
            headers = [x[0] for x in timelog]


# make directory empty before running
def empty_dir():
    if os.path.exists('/home/handsonhuang/gg/temp'):
        shutil.rmtree('/home/handsonhuang/gg/temp')
    os.mkdir('/home/handsonhuang/gg/temp')

# sort the lambda list by id and reset the id
def sort_lambda():
    global lambda_list
    lambda_list.sort(key=lambda x: x.id_)
    min_time = lambda_list[0].id_
    for l in lambda_list:
        l.id_ = l.id_ - min_time



empty_dir()
# sort_lambda()

# generate the thunk list
generate_thunk()

# create_cdf(myLog)
# create_runtime_csv()
# classify_dependency()

# plot_dependency_graph()
# plot_dependency_graph_without_command()
# create_thunk_csv()

# data_points.sort(key=lambda x: x[0])

# T0 = data_points[0][0]

# for d in data_points:
#     d[0] = d[0] - T0
#     d.append(sum(d[1:6]))

#print", ".join(['"timing_data" using %d t "%s"' % (i+1,x) for x, i in enumerate(headers) ])

# print("\t".join(["#"] + headers))
# for i, d in enumerate(data_points):
#     print(" ".join([str(i)] + [str(x) for x in d]))
