# camera-cuda 

### This is a repo that integrate multiple image processing algos on CUDA-based acceleration with bazel build

### Setup Environment

##### Step1:

###### Please make sure to download the executor file of Bazel version 6.2.0 or above

![Image text](https://github.com/cuiyixin555/camera-cuda/blob/master/bazel.jpg)

##### Step2:

###### Please add the path of bazel.exe as environment variables

![Image text](https://github.com/cuiyixin555/camera-cuda/blob/master/env.jpg)

##### Step3:

###### Please make sure to install VS2019 and above and add the path as environment variables

![Image text](https://github.com/cuiyixin555/camera-cuda/blob/master/vs2019_env.jpg)

##### Step4:

###### Please make sure to install CUDA 12.4 and cuDNN 9.2.0 and add the path as environment variables

![Image text](https://github.com/cuiyixin555/camera-cuda/blob/master/cuda_env1.jpg)

![Image text](https://github.com/cuiyixin555/camera-cuda/blob/master/cuda_env2.jpg)

##### Note:

###### Hardware Configuration: GeForce 4090 24G

#### Image Opencv API
$ bazel build //calculators/opencv/...

$ ./bazel-bin/calculators/opencv/read_image.exe

##### Output

![Image text](https://github.com/cuiyixin555/camera-cuda/blob/master/opencv_imread.png)

#### Image Rotater

$ bazel build //calculators/cuda/rotater/...  

$ ./bazel-bin/calculators/cuda/rotater/main.exe ./data/image/cat.bmp ./data/output/out_flip.bmp

##### Image Rotater Output

![Image text](https://github.com/cuiyixin555/camera-cuda/blob/master/data/output/out_flip.bmp)

#### Image Resize

$ bazel build //calculators/cuda/resize/...

$ ./bazel-bin/calculators/cuda/resize/main.exe

##### Image Resize Output

###### resize_nearest.png

![Image text](https://github.com/cuiyixin555/camera-cuda/blob/master/data/output/resize_nearest.png)

###### resize_bilinear.png

![Image text](https://github.com/cuiyixin555/camera-cuda/blob/master/data/output/resize_bilinear.png)

#### Image Edge Detector

$ bazel build //calculators/cuda/edge/...

$ ./bazel-bin/calculators/cuda/edge/main.exe ./data/image/cat.bmp ./data/output/out_edge.bmp

##### Image Edge Detector Output

![Image text](https://github.com/cuiyixin555/camera-cuda/blob/master/data/output/out_edge.bmp)

### Reference
All source code is based on this repo https://github.com/bazel-contrib/rules_cuda 

Opencv is 4.10.0 version from https://github.com/opencv/opencv/releases/download/4.10.0/opencv-4.10.0-windows.exe