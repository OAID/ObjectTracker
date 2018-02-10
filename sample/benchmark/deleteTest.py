import os
import random

path = './test'
files = os.listdir(path)
folder_list = []
truth_line_count = 0
file_count = 0
for file in files:
#        h = file.split('_',1)
#        temp = h[1].split('.',1)
         if file.find('.sh')<0:
		 folder_list.append(file)
print folder_list

for i in range(len(folder_list)):
        if os.path.isfile("test/%s/groundtruth_rect.txt"%folder_list[i]):
                truth_file = open("test/%s/groundtruth_rect.txt"%folder_list[i])
        else:
                 truth_file = open("test/%s/groundtruth_rect.1.txt"%folder_list[i])
        for truth_line in truth_file:
                truth_line_count += 1
	for filename in os.listdir("test/%s/img"%folder_list[i]):
		if filename.find('.jpg')>0:
			file_count = file_count + 1
	if truth_line_count != file_count:
		print ("%s error"%folder_list[i])
		print truth_line_count
		print file_count
		os.system("sudo mv test/%s error"%folder_list[i])
	truth_file.close()
	truth_line_count = 0
	file_count = 0

