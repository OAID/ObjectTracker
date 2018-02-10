# Accuracy test instructions

## Install dependencies

* Install OpenCV library for Python
```
sudo cp /usr/local/AID/opencv3.3.0/lib/python2.7/dist-packages/cv2.so /usr/local/lib/python2.7/dist-packages/cv2.so
```

* Install Matplotlib for Python
```
sudo apt-get install python-matplotlib
```

## Get the average fps value
Get the average fps value per 100 frames by using the "avfps" value in the window.

## Run accuracy test script

```
cp ~/ObjectTracker/build/bin/cftld ~/ObjectTracker/sample/benchmark
cd ~/ObjectTracker/sample/benchmark
wget -c ftp.openailab.net/dataset/ObjectTracker_Benchmark/test.tar --ftp-user=*** --ftp-password=*** --no-passive-ftp
tar -xvf test.tar
python cfg_gen.py
sh RunTracker.sh
python calc.py
```
You may need to contact <support@openailab.com> to access the dataset. 

## Add your own images to test dataset
```
1) save your images to ObjectTracker/sample/benchmark/test/[folder name]/img.
2) save the grountruth_rect.txt to ObjectTracker/sample/benchmark/test/[folder name].
3) run cfg_gen.py to generate the configuration file and RunTracker.sh.
4) run RunTracker.sh to start testing.
5) run calc.py to get the result.
```
