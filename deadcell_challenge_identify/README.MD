
# 识别死亡细胞中挑战区域的符文

- OBS Studio推流，服务器分析
  - 推流使用的是RTMP协议
  - RTMP协议传输的是FLV文件
  - [Nginx-rtmp-module](https://github.com/arut/nginx-rtmp-module/)+[Docker镜像](https://github.com/tiangolo/nginx-rtmp-docker)可以直接搭建一个支持直播的rtmp协议
- 从直播流获取flv视频帧
  - OpenCV！直接open一个rtmp协议的直播流
  - read可以读取RTMP直播流每一帧：https://docs.opencv.org/4.x/d8/dfe/classcv_1_1VideoCapture.html#details
  - 跳帧，grap只抓不decode：https://answers.opencv.org/question/24714/skipping-frames-in-videocapture/
- 分析每一帧
  - OpenCV！
  - 图形识别：https://pyimagesearch.com/2014/10/20/finding-shapes-images-using-python-opencv/ 不行
  - 图片相似度比较-Stack Overflow：https://datascience.stackexchange.com/questions/48642/how-to-measure-the-similarity-between-two-images
  - TemplateMatching：https://docs.opencv.org/2.4.13.7/doc/tutorials/imgproc/histograms/template_matching/template_matching.html
  - 裁剪图像：https://learnopencv.com/cropping-an-image-using-opencv/
- Windows Terminal`print("\x7")`能够响铃通知
  - flush
- 线程池：https://github.com/progschj/ThreadPool
