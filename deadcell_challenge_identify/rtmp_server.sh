sudo docker stop nginx-rtmp-test
sudo docker rm nginx-rtmp-test
sudo docker run -d --name nginx-rtmp-test -p 1935:1935 tiangolo/nginx-rtmp
