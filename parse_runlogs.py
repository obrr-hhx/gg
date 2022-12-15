#!/usr/bin/env python3

import os
from re import T
from socket import TIPC_NODE_SCOPE
import sys
import numpy as np
import matplotlib.pyplot as plt
from scipy import stats
import seaborn as sns
import pandas as pd
from graphviz import Digraph
import csv

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
        self.wrote_time = []
        self.read_time = []
        self.lifetime = 0
        self.read_lambda = []
        self.write_lambda = []
    def compute_lifetime(self):
        tmp_write = self.wrote_time
        tmp_read = self.read_time
        tmp_write.sort()
        tmp_read.sort(reverse=True)
        if self.hash_value == 'V5pB2cDVLOFGkh8oyHix1eURquEyL.xm12.JFisT.4kI000009f2':
            print(tmp_read)
            print("####################read lambda####################")
            lmbd = self.read_lambda[0]
            print("lambda id: ", lmbd.id_)
            print("lambd read thunk time: ", lmbd.read_thunk_time)
            print(tmp_write)
        if len(tmp_write) == 0:
            self.lifetime = tmp_read[0]
        elif len(tmp_read) != 0 and len(tmp_write) != 0:
            self.lifetime = tmp_read[0] - tmp_write[0]

class Lambda:
    def __init__(self) -> None:
        self.id_ = 0
        self.read_thunk_time = 0
        self.do_clean_time = 0
        self.get_dependencies_time = 0
        self.execute_time = 0
        self.upload_time = 0
        self.read_num = 0
        self.read_size = 0
        self.write_num = 0
        self.write_size = 0
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
    read_time = lambda_entity.id_ + lambda_entity.read_thunk_time + lambda_entity.do_clean_time + lambda_entity.get_dependencies_time
    return read_time

def compute_lambda_write_time(lmbd):
    write_time = lmbd.id_ + lmbd.read_thunk_time + lmbd.do_clean_time + lmbd.get_dependencies_time + lmbd.execute_time + lmbd.upload_time
    return write_time

def generate_thunk():
    global lambda_list
    global thunk_list
    for lmbd in lambda_list:
        for h in lmbd.read_hash:
            t , thunk = check_in_hashList(h)
            if not t:
                thunk = Thunk()
                thunk.hash_value = h
                thunk.read_time.append(compute_lambda_read_time(lmbd))
                thunk.read_lambda.append(lmbd)
                thunk_list.append(thunk)
            else:
                thunk.read_time.append(compute_lambda_read_time(lmbd))
                thunk.read_lambda.append(lmbd)
        for h in lmbd.write_hash:
            t , thunk = check_in_hashList(h)
            if not t:
                thunk = Thunk()
                thunk.hash_value = h
                thunk.wrote_time.append(compute_lambda_write_time(lmbd))
                thunk.write_lambda.append(lmbd)
                thunk_list.append(thunk)
            else:
                thunk.wrote_time.append(compute_lambda_write_time(lmbd))
                thunk.write_lambda.append(lmbd)


def create_lambda(timelog):
    lmbd = Lambda()
    for log in timelog:
        if log[0] == 'started':
            lmbd.id_ = log[1]
        elif log[0] == 'read_thunk':
            lmbd.read_thunk_time = log[1]
        elif log[0] == 'do_cleanup':
            lmbd.do_clean_time = log[1]
        elif log[0] == 'get_dependencies':
            lmbd.get_dependencies_time = log[1]
        elif log[0] == 'execute':
            lmbd.execute_time = log[1]
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
            lmbd.read_hash.append(log[1])
            # lmbd.get_thunk.append(log[1])
        elif log[0] == 'upload_hash':
            lmbd.write_hash.append(log[1])
            # lmbd.upload_thunk.append(log[1])       
    global lambda_list
    lambda_list.append(lmbd)
         
'''
    Generate the dependency graphy, DAG
'''
def classfy_dependency():
    
    global lambda_list
    for current_lmbd in lambda_list:
        for lmbd_x in lambda_list:
            if(current_lmbd != lmbd_x):
                for hash_v in current_lmbd.write_hash:
                    if hash_v in lmbd_x.read_hash:
                        current_lmbd.children_lambda.append(lmbd_x)
                        lmbd_x.parent_lambda.append(current_lmbd)

def traverse_create(g, parent, child):
    label = format_label(child)
    g.node(str(child.id_), label)
    g.edge(str(parent.id_), str(child.id_))
    if len(child.children_lambda) != 0:
        for chd in child.children_lambda:
            traverse_create(g, child, chd)

def format_label(node) -> str:
    lb_id = 'id: ' + str(node.id_) + '\n'
    lb_read_num = 'read num: ' + str(node.read_num) + '\n'
    lb_read_size = 'read size: ' + str(node.read_size) + ' bytes' + '\n'
    lb_write_num = 'write num: ' + str(node.write_num) + '\n'
    lb_write_size = 'write size: ' + str(node.write_size) + ' bytes'
    label = lb_id+lb_read_num+lb_read_size+lb_write_num+lb_write_size
    return label

def plot_dependency_graph():
    global lambda_list
    root_lambda = []
    g = Digraph("Dependency",strict=True)
    for lmbd in lambda_list:
        if len(lmbd.parent_lambda) == 0:
            root_lambda.append(lmbd)
    for node in root_lambda:
        label = format_label(node)
        g.node(str(node.id_),label=label)
    for node in root_lambda:
        if len(node.children_lambda) != 0:
            for chd_node in node.children_lambda:
                traverse_create(g, node, chd_node)
    
    g.render(datadir.split('/')[5]+'_dep.gv', directory="/home/handsonhuang/gg/temp/",format='svg')



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
    data_log = {'read_num':[], 'write_num':[], 'read_size':[], 'write_size':[]}
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
    plot_cdf(data_log)


''''
    Create the csv file about thunk's read and wrote lambdas and its lifetime

    thunk
    hash_value    read_lambda    write_lambda    life_time
'''
def create_thunk_csv():
    global thunk_list
    headers_read = ['hash', 'read_lambda']
    headers_write = ['hash', 'write_lambda']
    headers_life = ['hash', 'read_num', 'write_num','lifetime']
    # create the hash-read
    with open('/home/handsonhuang/gg/temp/hash_read.csv', 'w', encoding='utf8', newline='') as f:
        writer = csv.writer(f)
        rows = []
        for thunk in thunk_list:
            if len(thunk.read_lambda) != 0 and len(thunk.write_lambda) != 0:
                row = []
                row.append(thunk.hash_value)
                for lmbda in thunk.read_lambda:
                    row.append(str(lmbda.id_))
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
                    row.append(str(lmbda.id_))
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
            row.append(lmbd.id_)
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
    parse the logfile in given path
'''
for logfile in os.listdir(datadir):
    path = os.path.join(datadir, logfile)

    with open(path, "r") as log:
        timelog = []

        for line in log:
            if not timelog:
                timelog += [('started', int(line))]
            else:
                if line.split()[1] != 'Numbers' and line.split()[1] != 'Sizes' and line.split()[1] != 'Hash':
                    # print(line.split())
                    if line.split()[0] == 'get_hash' or line.split()[0] == 'upload_hash':
                        timelog += [(line.split()[0], (line.split()[1]))]
                    else:
                        timelog += [(line.split()[0], int(line.split()[1]))]
        create_lambda(timelog)
        myLog += timelog
        point = [x[1] for x in timelog]
        data_points += [point]

        if not headers:
            headers = [x[0] for x in timelog]


create_cdf(myLog)

lambda_list.sort(key=lambda x: x.id_)
min_time = lambda_list[0].id_
for l in lambda_list:
    l.id_ = l.id_ - min_time

create_runtime_csv()
classfy_dependency()
plot_dependency_graph()

generate_thunk()
for thunk in thunk_list:
    thunk.compute_lifetime()

create_thunk_csv()


data_points.sort(key=lambda x: x[0])

T0 = data_points[0][0]

for d in data_points:
    d[0] = d[0] - T0
    d.append(sum(d[1:6]))

#print", ".join(['"timing_data" using %d t "%s"' % (i+1,x) for x, i in enumerate(headers) ])

# print("\t".join(["#"] + headers))
# for i, d in enumerate(data_points):
#     print(" ".join([str(i)] + [str(x) for x in d]))
