# camera-cuda 

### This is repo for image processing with bazel build

### How to run
#### Image Rotater
$ cd examples

$ bazel build //rotater/...  

$ ./bazel-bin/rotater/main.exe ./data/image/cat.bmp ./data/output/out_flip.bmp

#### Image Edge Detector

$ cd examples

$ bazel build //edge/...

$ ./bazel-bin/edge/main.exe ./data/image/cat.bmp ./data/output/out_edge.bmp

### Input  
![Image text](https://github.com/cuiyixin555/camera-cuda/blob/master/examples/data/image/cat.bmp)

### Output  
![Image text](https://github.com/cuiyixin555/camera-cuda/blob/master/examples/data/output/out_flip.bmp)

![Image text](https://github.com/cuiyixin555/camera-cuda/blob/master/examples/data/output/out_edge.bmp)
