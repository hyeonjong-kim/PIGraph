import os
import argparse

parser = argparse.ArgumentParser(description='Resource Analysis Tools')
parser.add_argument('--cpu_path', required=True, help='cpu log file path')
parser.add_argument('--mem_path', required=True, help='mem log file path')
parser.add_argument('--host_num', required=True, help='number of host')
parser.add_argument('--file_name', required=True, help='file name')

args = parser.parse_args()
cpu_dic = {}
mem_dic = {}

for filename in os.listdir(args.cpu_path):
    read_count = 0
    result_l = []
    with open(os.path.join(args.cpu_path, filename), 'r') as f:
        if str(filename).split('_')[2] == str(args.file_name) + ".log" and  str(filename).split('_')[4] == str(args.host_num):
            while True:
                read_line = f.readline()
                if not read_line: break
                if read_count >= 3:
                    l = read_line.split(' ')
                    l = [i for i in l if i not in {''}]
                    if len(l) > 9:
                        result_l.append(float(l[9]))
                
                read_count += 1

            file_name = os.path.join(args.cpu_path, filename).split('/')   
            if len(result_l) != 0 :
                cpu_dic[file_name[-1]] = sum(result_l)/len(result_l)
            else:
                cpu_dic[file_name[-1]] = 0

for filename in os.listdir(args.mem_path):
    read_count = 0
    result_l = []
    with open(os.path.join(args.mem_path, filename), 'r') as f:
        if str(filename).split('_')[2] == (str(args.file_name) + ".log") and  str(filename).split('_')[4] == str(args.host_num):
            while True:
                read_line = f.readline()
                if not read_line: break
                if read_count >= 3:
                    l = read_line.split(' ')
                    l = [i for i in l if i not in {''}]
                    if len(l) > 9:
                        result_l.append(float(l[9]))
                read_count += 1
            file_name = os.path.join(args.mem_path, filename).split('/')
            if len(result_l) != 0 :
                mem_dic[file_name[-1]] = sum(result_l)/len(result_l)
            else:
                mem_dic[file_name[-1]] = 0

print(cpu_dic)
print(mem_dic)

cpu_dic = sorted(cpu_dic.items())
mem_dic = sorted(mem_dic.items())

with open("test_log_Synthesis_cpu_" + str(args.file_name) + "_" + str(args.host_num), 'w') as f:
    for i in cpu_dic:
        w_line =  str(i[0]) + " : " + str(i[1]) + "\n"
        f.write(w_line)


with open("test_log_Synthesis_mem_" + str(args.file_name) + "_" + str(args.host_num), 'w') as f:
    for i in mem_dic:
        w_line =  str(i[0]) + " : " + str(i[1]) + "\n"
        f.write(w_line)