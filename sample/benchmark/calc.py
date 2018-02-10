import os
import random
import matplotlib.pyplot as plt
from matplotlib import style

path = './results/KCF'
files = os.listdir(path)
folder_list = []
truth_list = []
result_list = []
temp_list = []
accuracy_list = []
accuracy_all_list = []
iou_list = []
iou_all_list = []
successrate_list = []
successrate_all_list = []
random_count = 5
debug_count = 0
truth_line_count = 0
results_line_count = 0
icount_all = 0
for file in files:  
	h = file.split('_',1)
        temp = h[1].split('.',1)
        folder_list.append(temp[0])
print folder_list

for i in range(len(folder_list)):
	truth_line_count = 0	
	results_line_count = 0
	truth_list = []	
	result_list = []
	if os.path.isfile("test/%s/groundtruth_rect.txt"%folder_list[i]):
		truth_file = open("test/%s/groundtruth_rect.txt"%folder_list[i])
	else:
		truth_file = open("test/%s/groundtruth_rect.1.txt"%folder_list[i])
	for truth_line in truth_file:
		temp_list = []
		truth_line = truth_line.strip('\n')
		if truth_line.find(',')>=0:
			for icount1 in range(0,4):
				temp_list.append(truth_line.split(',',truth_line.count(','))[icount1])
		else:
			for icount2 in range(0,4):
				temp_list.append(truth_line.split()[icount2])	
		truth_list.append(temp_list)
#		debug_count += 1
		truth_line_count += 1
	
	results_file = open(path+"/results_%s.txt"%folder_list[i])
	for results_line in results_file:
		temp_list = []
		results_line = results_line.strip('\n')
		if results_line.find(',')>=0:
			for icount1 in range(1,5):
				temp_list.append(results_line.split(',',results_line.count(','))[icount1])
		else:
			for icount2 in range(1,5):
				temp_list.append(results_line.split()[icount2])	
		result_list.append(temp_list)	
#		print temp_list
		results_line_count += 1
	
	if len(truth_list) == len(result_list):
		print folder_list[i]
		defeat = 0
		iousum = 0
		successrate_list = []
		iou_list = []
		accuracy_list = []
		print accuracy_list
		for icount3 in range(1,len(truth_list)):
			xt1 = float(truth_list[icount3][0])
			yt1 = float(truth_list[icount3][1])
			wt = float(truth_list[icount3][2])
			ht = float(truth_list[icount3][3])
			xt2 = xt1 + wt
			yt2 = yt1 + ht
			if (result_list[icount3][0].find("NaN") < 0 and result_list[icount3][1].find("NaN") < 0 and result_list[icount3][2].find("NaN") < 0 and result_list[icount3][3].find("NaN") < 0):
				xr1 = float(result_list[icount3][0])
 				yr1 = float(result_list[icount3][1])
				wr = float(result_list[icount3][2])
				hr = float(result_list[icount3][3])
				xr2 = xr1 + wr
				yr2 = yr1 + hr
				areat = (wt + 1) * (ht + 1)
				arear = (wr + 1) * (hr + 1)
				x0 = max(xt1,xr1)
				x1 = min(xt2,xr2)
				y0 = max(yt1,yr1)
				y1 = min(yt2,yr2)
				if (x0 >= x1 or y0 >= y1):
					iou = 0
				else:
					areax = (x1 - x0 + 1)*(y1 - y0 + 1)
					iou = float(areax)/(areat+arear-areax)
			else:
				iou = 0
#			if iou == 0:
#				defeat += 1
#			successrate_list.append(float(icount3-defeat)/float(icount3))
			iou_list.append(iou)
			iousum = iousum + iou
			accuracy_list.append(iousum/icount3)
#		successrate_all_list.append(successrate_list)	
		
		iou_all_list.append(iou_list)
		print len(accuracy_list)
		accuracy_all_list.append(accuracy_list)
		
	else:
		print " error"
	
successrate_loop = range(0,11)
for fcount in successrate_loop:
	icount_all = 0
	defeat = 0 
	for icount4 in range(len(iou_all_list)):
		for icount5 in range(len(iou_all_list[icount4])):
			icount_all += 1
			if iou_all_list[icount4][icount5] <= float(fcount)/10:
				defeat += 1
	successrate_all_list.append(float(icount_all-defeat)/float(icount_all))

path = './results/DSST'
files = os.listdir(path)
folder_list = []
truth_list = []
result_list = []
temp_list = []
accuracy_list = []
accuracy_all_list2 = []
iou_list = []
iou_all_list2 = []
successrate_list = []
successrate_all_list2 = []
random_count = 5
debug_count = 0
truth_line_count = 0
results_line_count = 0
icount_all = 0
icount1 = 0
icount2 = 0
icount3 = 1
icount4 = 0
icount5 = 0

for file in files:  
	h = file.split('_',1)
        temp = h[1].split('.',1)
        folder_list.append(temp[0])
print folder_list

for i in range(len(folder_list)):
	truth_line_count = 0	
	results_line_count = 0
	truth_list = []	
	result_list = []
	if os.path.isfile("test/%s/groundtruth_rect.txt"%folder_list[i]):
		truth_file = open("test/%s/groundtruth_rect.txt"%folder_list[i])
	else:
		truth_file = open("test/%s/groundtruth_rect.1.txt"%folder_list[i])
	for truth_line in truth_file:
		temp_list = []
		truth_line = truth_line.strip('\n')
		if truth_line.find(',')>=0:
			for icount1 in range(0,4):
				temp_list.append(truth_line.split(',',truth_line.count(','))[icount1])
		else:
			for icount2 in range(0,4):
				temp_list.append(truth_line.split()[icount2])	
		truth_list.append(temp_list)
#		debug_count += 1
		truth_line_count += 1
	
	results_file = open(path+"/results_%s.txt"%folder_list[i])
	for results_line in results_file:
		temp_list = []
		results_line = results_line.strip('\n')
		if results_line.find(',')>=0:
			for icount1 in range(1,5):
				temp_list.append(results_line.split(',',results_line.count(','))[icount1])
		else:
			for icount2 in range(1,5):
				temp_list.append(results_line.split()[icount2])	
		result_list.append(temp_list)	
#		print temp_list
		results_line_count += 1
	
	if len(truth_list) == len(result_list):
		print folder_list[i]
		defeat = 0
		iousum = 0
		successrate_list = []
		iou_list = []
		accuracy_list = []
		print accuracy_list
		for icount3 in range(1,len(truth_list)):
			xt1 = float(truth_list[icount3][0])
			yt1 = float(truth_list[icount3][1])
			wt = float(truth_list[icount3][2])
			ht = float(truth_list[icount3][3])
			xt2 = xt1 + wt
			yt2 = yt1 + ht
			if (result_list[icount3][0].find("NaN") < 0 and result_list[icount3][1].find("NaN") < 0 and result_list[icount3][2].find("NaN") < 0 and result_list[icount3][3].find("NaN") < 0):
				xr1 = float(result_list[icount3][0])
 				yr1 = float(result_list[icount3][1])
				wr = float(result_list[icount3][2])
				hr = float(result_list[icount3][3])
				xr2 = xr1 + wr
				yr2 = yr1 + hr
				areat = (wt + 1) * (ht + 1)
				arear = (wr + 1) * (hr + 1)
				x0 = max(xt1,xr1)
				x1 = min(xt2,xr2)
				y0 = max(yt1,yr1)
				y1 = min(yt2,yr2)
				if (x0 >= x1 or y0 >= y1):
					iou = 0
				else:
					areax = (x1 - x0 + 1)*(y1 - y0 + 1)
					iou = float(areax)/(areat+arear-areax)
			else:
				iou = 0
#			if iou == 0:
#				defeat += 1
#			successrate_list.append(float(icount3-defeat)/float(icount3))
			iou_list.append(iou)
			iousum = iousum + iou
			accuracy_list.append(iousum/icount3)
#		successrate_all_list.append(successrate_list)	
		
		iou_all_list2.append(iou_list)
		print len(accuracy_list)
		accuracy_all_list2.append(accuracy_list)
		
	else:
		print " error"
	
successrate_loop = range(0,11)
for fcount in successrate_loop:
	icount_all = 0
	defeat = 0 
	for icount4 in range(len(iou_all_list2)):
		for icount5 in range(len(iou_all_list2[icount4])):
			icount_all += 1
			if iou_all_list2[icount4][icount5] <= float(fcount)/10:
				defeat += 1
	successrate_all_list2.append(float(icount_all-defeat)/float(icount_all))
	

x_loop_list = [0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1]
style.use('ggplot')
#fig = plt.figure(i)
plt.figure()
#ax1 = fig.add_subplot(121)	
#plt.xlabel('The total number of frames')
#plt.ylabel('The average accuracy')
#plt.title('Average accuracy curve of %s'%folder_list[i])
#plt.grid(True)
#temp_list1 = []
#for icount6 in range(len(iou_all_list)):
#                for icount7 in range(len(iou_all_list[icount6])):
#			temp_list1.append(iou_all_list[icount6][icount7])
#temp_list1.sort()

#ax1.plot(range(len(temp_list1)),temp_list1)

#ax2 = fig.add_subplot(122)
plt.xlabel('Overlap threshold')
plt.ylabel('Success rate')
plt.title('Success plots of OPE')
plt.grid(True)
plt.plot(x_loop_list,successrate_all_list,"x-",label = "KCF")
plt.plot(x_loop_list,successrate_all_list2,"+-",label = "DSST")
plt.legend(bbox_to_anchor=(1.0, 1), loc=1, borderaxespad=0.)
plt.show()
