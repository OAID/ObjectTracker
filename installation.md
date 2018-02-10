# Installation

## Install dependencies

* Install CMake:
Skip this step if OpenCV3.3 has been installed
```
sudo apt-get install cmake
```

* Download OpenCV3.3
```
wget https://github.com/opencv/opencv/archive/3.3.0.zip --no-check-certificate
unzip 3.3.0.zip
```

* Download "gen-pkg-config-pc"
```
wget ftp://ftp.openailab.net/tools/script/gen-pkg-config-pc.sh
chmod +x ./gen-pkg-config-pc.sh
```

* Install OpenCV3.3
```
cd opencv-3.3.0
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local/AID/opencv3.3.0 ..
sudo make install
```

* Configure The Libraries
```
sudo ~/gen-pkg-config-pc.sh /usr/local/AID
```

## ObjectTracker Compile

```
cd ObjectTracker
mkdir build
cd build
cmake ..
make
```

## Camera live demo 
* Please ensure camera is connected

```
cd ObejectTracker/build/bin
./cftld
```

## Command line options
### Synopsis
`bin/cftld [option arguments] [Arguments]`

### option arguments
* `[-x]` Use DSSTcpp instead of KCFcpp as short-term tracker. KCFcpp is the default short-term tracker.
* `[-a <startFrameNumber>]` Video starts at the _startFrameNumber_.
* `[-b <x,y,w,h>]` Initial bounding box.
* `[-d <device>]` Selects input device: _device_=(_IMGS_|_CAM_|_VID_|_STREAM_)  
    _IMGS_: Capture from images.  
    _CAM_: Capture from connected camera.  
    _VID_: Capture from a video.  
    _STREAM_: Capture from RTSP stream.
* `[-f]` Shows foreground.
* `[-i <path>]` _path_ to the images or to the video.
* `[-j <number>]` Shows trajectory for the last _number_ frames.
* `[-h]` Shows help.
* `[-n <number>]` Specifies the video device to use (defaults to 0). Useful to select a different camera when multiple cameras are connected.
* `[-p path]` Prints results into the file _path_.
* `[-s]` If set, user can select initial bounding box.
* `[-t <theta>]` Threshold for determining positive results.
* `[-z <lastFrameNumber>]` Video ends at the _lastFrameNumber_.
	If _lastFrameNumber_ is 0 or the option argument isn't specified means
	all frames are taken.

### Arguments
`[CONFIG_FILE]` path to config file

## Config files
Refer to the sample folder to see usage and config file examples.

