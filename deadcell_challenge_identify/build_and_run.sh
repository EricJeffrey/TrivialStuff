echo 'building'
g++ -O2 main.cpp -I /usr/include/opencv4/ -lopencv_core -lopencv_videoio -lopencv_imgcodecs -lopencv_imgproc -lopencv_highgui -lpthread
echo 'starting'
./a.out
