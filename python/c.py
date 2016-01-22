#!/usr/bin/python
import os
import xlsxwriter
def txt_wrap_by(start_str, end, html):
    start =html.find(start_str)
    if start >= 0:
        start += len(start_str)
        end = html.find(end,start)
        if end >=0:
            return html[start:end].strip()
os.system("rm -rf chart_line.xlsx  fail.txt success.txt run.txt run_time.txt")

#calculate fail call info
os.system("cat *|grep \"Failed Total Calls ----------\">fail.txt")
os.system("sort -u fail.txt")
#calculate success call info
os.system("cat *|grep \"Total Succeeded Calls -------\">success.txt")
os.system("sort -u success.txt")
#calculate total call info
os.system("cat *|grep \"Total Run Calls -------------\">run.txt")
os.system("sort -u run.txt")
#calculate total run time
os.system("cat *|grep \"Total Running Time Min ------\">run_time.txt")
os.system("sort -u run_time.txt")
# whole info dictionary
call_dic = {}

with open('run_time.txt','r') as call_data:
	for i in call_data:
		sim_num = i.split(":")[0]
		if not 'H248' in str(sim_num):
			continue
		fail_num_list = i.split(" ")
		if len(fail_num_list) < 7:
			continue
		fail_num = float(fail_num_list[7].strip())

		#print(fail_num)
		if not sim_num in call_dic:
			call_dic[sim_num] = {}
			if not 'arun_time' in call_dic[sim_num]:
				call_dic[sim_num]['arun_time'] = []
				call_dic[sim_num]['arun_time'].append(fail_num)
			else:
				call_dic[sim_num]['arun_time'].append(fail_num)
		else:
			if not 'arun_time' in call_dic[sim_num]:
				call_dic[sim_num]['arun_time'] = []
				call_dic[sim_num]['arun_time'].append(fail_num)
			else:
				call_dic[sim_num]['arun_time'].append(fail_num)

with open('fail.txt','r') as call_data:
	for i in call_data:
		sim_num = i.split(":")[0]
		if not 'H248' in str(sim_num):
			continue
		fail_num_list = i.split(" ")
		if len(fail_num_list) < 7:
			continue
		fail_num = int(fail_num_list[6].strip())

		#print(fail_num)
		if not sim_num in call_dic:
			call_dic[sim_num] = {}
			if not 'fail' in call_dic[sim_num]:
				call_dic[sim_num]['fail'] = []
				call_dic[sim_num]['fail'].append(fail_num)
			else:
				call_dic[sim_num]['fail'].append(fail_num)
		else:
			if not 'fail' in call_dic[sim_num]:
				call_dic[sim_num]['fail'] = []
				call_dic[sim_num]['fail'].append(fail_num)
			else:
				call_dic[sim_num]['fail'].append(fail_num)

with open('success.txt','r') as call_data:
	for i in call_data:
		sim_num = i.split(":")[0]
		if not 'H248' in str(sim_num):
			continue
		fail_num_list = i.split(" ")
		if len(fail_num_list) < 7:
			continue
		fail_num = int(fail_num_list[6].strip())

		#print(fail_num)
		if not sim_num in call_dic:
			call_dic[sim_num] = {}
			if not 'success' in call_dic[sim_num]:
				call_dic[sim_num]['success'] = []
				call_dic[sim_num]['success'].append(fail_num)
			else:
				call_dic[sim_num]['success'].append(fail_num)
		else:
			if not 'success' in call_dic[sim_num]:
				call_dic[sim_num]['success'] = []
				call_dic[sim_num]['success'].append(fail_num)	
			else:
				call_dic[sim_num]['success'].append(fail_num)

with open('run.txt','r') as call_data:
	for i in call_data:
		sim_num = i.split(":")[0]
		if not 'H248' in str(sim_num):
			continue
		fail_num_list = i.split(" ")
		if len(fail_num_list) < 7:
			continue
		fail_num = int(fail_num_list[6].strip())

		#print(fail_num)
		if not sim_num in call_dic:
			call_dic[sim_num] = {}
			if not 'run' in call_dic[sim_num]:
				call_dic[sim_num]['run'] = []
				call_dic[sim_num]['run'].append(fail_num)
			else:
				call_dic[sim_num]['run'].append(fail_num)
		else:
			if not 'run' in call_dic[sim_num]:
				call_dic[sim_num]['run'] = []
				call_dic[sim_num]['run'].append(fail_num)
			else:
				call_dic[sim_num]['run'].append(fail_num)			




print call_dic


#sim key list
sim_list = list(call_dic.keys())
#saved data list
data_list = list(call_dic[sim_list[0]])
data_list.sort()
#col counter
col =0
#how many sims
sim_count = len(call_dic)
#how many data
data_count = len(call_dic[sim_list[0]]['run'])
#how many data types
data_type_count = len(data_list)

workbook = xlsxwriter.Workbook('chart_line.xlsx')
worksheet = workbook.add_worksheet()
bold = workbook.add_format({'bold': 1})

tmp_col = 0

for key in sim_list:
	chart = workbook.add_chart({'type': 'line'})
	for key_data in data_list:
		worksheet.write_column(1, col , call_dic[key][key_data])
		worksheet.write_string(0, col, str(key_data))
		if key_data == 'arun_time':
			tmp_col = col
		else:
			chart.add_series({
				'name':       ['Sheet1', 0, col],
				'categories': ['Sheet1', 1, tmp_col, data_count, tmp_col],
				'values':     ['Sheet1', 1, col, data_count, col],
			})
		col = col + 1
	# Add a chart title and some axis labels.
	sim_title = 'data of sim ' + str(key)
	chart.set_title ({'name': sim_title})
	chart.set_x_axis({'name': 'running time'})
	chart.set_y_axis({'name': 'calls'})	
			
	# Set an Excel chart style. Colors with white outline and shadow.
	chart.set_style(10)
	worksheet.insert_chart(int(col/data_type_count)*16, 0, chart)
	#chart.clear()


workbook.close()

