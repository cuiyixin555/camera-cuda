# camera-cuda 
This is a repo that integrate multiple image processing algos on CUDA-based acceleration with bazel build

### Setup Environment

##### Step1: Please make sure to download the executor file of Bazel version 6.2.0 or above

![Image text](https://github.com/cuiyixin555/camera-cuda/blob/master/assets/bazel.jpg)

##### Step2: Please add the path of bazel.exe as environment variables

![Image text](https://github.com/cuiyixin555/camera-cuda/blob/master/assets/env.jpg)

##### Step3: Please make sure to install VS2019 and above and add the path as environment variables

![Image text](https://github.com/cuiyixin555/camera-cuda/blob/master/assets/vs2019_env.jpg)

##### Step4: Please make sure to install CUDA 12.4 and cuDNN 9.2.0 and add the path as environment variables

![Image text](https://github.com/cuiyixin555/camera-cuda/blob/master/assets/cuda_env1.jpg)

![Image text](https://github.com/cuiyixin555/camera-cuda/blob/master/assets/cuda_env2.jpg)

##### Step5: Please make sure you downloads camera-supp 

$ git clone https://github.com/cuiyixin555/camera-supp

$ cd camera-supp

$ git lfs pull

##### Step6: Please make sure the repo of camera-cuda and camera-supp in same directory

![Image text](https://github.com/cuiyixin555/camera-cuda/blob/master/assets/path_env.jpg)

##### Step7: Please add the path of camera-supp as Win11 environment value

The bazel.exe path is 

C:\\Users\\admin\\OneDrive\\Desktop\\workspace\\camera-supp\\build_tools\\windows\\bazel.exe

![Image text](https://github.com/cuiyixin555/camera-cuda/blob/master/assets/supp_env1.jpg)

##### Note: Hardware Configuration is GeForce 4090 24G

### How to Run

#### Image Opencv API

##### Image Read with OpenCV

$ bazel build //calculators/opencv/...

$ ./bazel-bin/calculators/opencv/read_image.exe

##### Output

![Image text](https://github.com/cuiyixin555/camera-cuda/blob/master/assets/opencv_imread.png)

#### Image CUDA API

##### Rotater

$ bazel build //calculators/cuda/rotater/...  

$ ./bazel-bin/calculators/cuda/rotater/main.exe ./data/image/cat.bmp ./data/output/out_flip.bmp

##### Rotater Output

![Image text](https://github.com/cuiyixin555/camera-cuda/blob/master/data/output/out_flip.bmp)

##### Resize

$ bazel build //calculators/cuda/resize/...

$ ./bazel-bin/calculators/cuda/resize/main.exe

##### Resize Output

###### resize_nearest.png

![Image text](https://github.com/cuiyixin555/camera-cuda/blob/master/data/output/resize_nearest.png)

###### resize_bilinear.png

![Image text](https://github.com/cuiyixin555/camera-cuda/blob/master/data/output/resize_bilinear.png)

##### Edge Detector

$ bazel build //calculators/cuda/edge/...

$ ./bazel-bin/calculators/cuda/edge/main.exe ./data/image/cat.bmp ./data/output/out_edge.bmp

##### Edge Detector Output

![Image text](https://github.com/cuiyixin555/camera-cuda/blob/master/data/output/out_edge.bmp)

### Reference
All source code is based on this repo https://github.com/bazel-contrib/rules_cuda 

Opencv is 4.10.0 version from https://github.com/opencv/opencv/releases/download/4.10.0/opencv-4.10.0-windows.exe
