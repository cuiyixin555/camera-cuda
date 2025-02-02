# camera-cuda 

### This is a repo that integrate multiple image processing algos on CUDA-based acceleration with bazel build

#### Image Opencv API
$ bazel build //calculators/opencv/...

$ ./bazel-bin/calculators/opencv/read_image.exe

##### Output

![Image text](https://github.com/cuiyixin555/camera-cuda/blob/master/opencv_imread.png)

#### Image Rotater
$ cd examples

$ bazel build //rotater/...  

$ ./bazel-bin/rotater/main.exe ./data/image/cat.bmp ./data/output/out_flip.bmp

##### Image Rotater Output

![Image text](https://github.com/cuiyixin555/camera-cuda/blob/master/examples/data/output/out_flip.bmp)

#### Image Edge Detector

$ cd examples

$ bazel build //edge/...

$ ./bazel-bin/edge/main.exe ./data/image/cat.bmp ./data/output/out_edge.bmp

##### Image Edge Detector Output

![Image text](https://github.com/cuiyixin555/camera-cuda/blob/master/examples/data/output/out_edge.bmp)

### Reference
All source code is based on this repo https://github.com/bazel-contrib/rules_cuda 

Opencv is 4.10.0 version from https://github.com/opencv/opencv/releases/download/4.10.0/opencv-4.10.0-windows.exe