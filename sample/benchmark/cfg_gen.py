import os
import random

path = './test'
files = os.listdir(path)
folder_list = []
random_count = 0 
for file in files:  
 
    m = os.path.join(path,file)  

    if (os.path.isdir(m)):  
        h = os.path.split(m)  
#        print h[1]  
        folder_list.append(h[1])  
# print folder_list
random_count = len(folder_list)
for i in range(len(folder_list)):
	ran_num = random.randint(0,len(folder_list)-1)
	folder_list[i],folder_list[ran_num] = folder_list[ran_num], folder_list[i]

random_list = folder_list[0:random_count]
print(random_list)

if(~os.path.exists("config")):
	os.system("mkdir config")
if(~os.path.exists("config/KCF")):
	os.system("mkdir config/KCF")
if(~os.path.exists("config/DSST")):
	os.system("mkdir config/DSST")
if(~os.path.exists("results")):
        os.system("mkdir results")
if(~os.path.exists("results/KCF")):
        os.system("mkdir results/KCF")
if(~os.path.exists("results/DSST")):
        os.system("mkdir results/DSST")

shfile1 = open("RunTracker.sh","w")
#shfile2 = open("KCFTracker.sh","w")
for i in range(len(random_list)):
	if os.path.isfile("test/%s/groundtruth_rect.txt"%random_list[i]):
		truth_file = open("test/%s/groundtruth_rect.txt"%random_list[i])
	else:
		truth_file = open("test/%s/groundtruth_rect.1.txt"%random_list[i])
	line = truth_file.readline()
	line = line.strip('\n')
	truth_file.close()
	File = open("config/KCF/image_sequence_%s"%random_list[i]+".cfg", "w")
	File.write("seed = 0;" + "\n")
	if line.find(',')==-1:
		File.write("initialBoundingBox = [%s,%s,%s,%s];"%(line.split()[0],line.split()[1],line.split()[2],line.split()[3]) + "\n") 
	else:
		File.write("initialBoundingBox = [%s];"%line + "\n")
	File.write("showNotConfident = true;" + "\n") 
	File.write("saveOutput = true;" + "\n") 
	File.write('saveDir = "%s";'%random_list[i] + "\n") 
	File.write('printResults = "results/KCF/results_%s.txt";'%random_list[i] + "\n")
	File.write("showOutput = true;"+ "\n") 
	File.write("# useDsstTracker = true;" + "\n\n") 
	File.write("acq: {" + "\n") 
	File.write('	method = "IMGS";' + "\n") 
	File.write('	imgPath = "test/%s/img/%%.4d.jpg";'%random_list[i] + "\n") 
	File.write("};" + "\n") 
	File.close()
	File = open("config/DSST/image_sequence_%s"%random_list[i]+".cfg", "w")
	File.write("seed = 0;" + "\n")
	if line.find(',')==-1:
		File.write("initialBoundingBox = [%s,%s,%s,%s];"%(line.split()[0],line.split()[1],line.split()[2],line.split()[3]) + "\n") 
	else:
		File.write("initialBoundingBox = [%s];"%line + "\n")
	File.write("showNotConfident = true;" + "\n") 
	File.write("saveOutput = true;" + "\n") 
	File.write('saveDir = "%s";'%random_list[i] + "\n") 
	File.write('printResults = "results/DSST/results_%s.txt";'%random_list[i] + "\n")
	File.write("showOutput = true;"+ "\n") 
	File.write("# useDsstTracker = true;" + "\n\n") 
	File.write("acq: {" + "\n") 
	File.write('	method = "IMGS";' + "\n") 
	File.write('	imgPath = "test/%s/img/%%.4d.jpg";'%random_list[i] + "\n") 
	File.write("};" + "\n") 
	File.close()
	shfile1.write("./cftld -x config/DSST/image_sequence_%s.cfg"%random_list[i] + "\n")
	shfile1.write("./cftld config/KCF/image_sequence_%s.cfg"%random_list[i] + "\n")
shfile1.close()
#shfile2.close()
