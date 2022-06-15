#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/img_hash.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"

#include "thread_pool.hpp"

#include <chrono>
#include <unistd.h>

using cv::Mat;
using cv::VideoCapture;
using namespace std;

inline int64_t nowInMs() {
    return chrono::duration_cast<chrono::milliseconds>(
               chrono::system_clock::now().time_since_epoch())
        .count();
}

double scoreMatching(const Mat &img, const Mat &templ) {
    cv::TemplateMatchModes match_method = cv::TemplateMatchModes::TM_CCOEFF_NORMED;
    // match_method = cv::TemplateMatchModes::TM_CCORR_NORMED;
    // match_method = cv::TemplateMatchModes::TM_SQDIFF;
    // match_method = cv::TemplateMatchModes::TM_CCORR;
    // match_method = cv::TemplateMatchModes::TM_SQDIFF_NORMED;
    // match_method = cv::TemplateMatchModes::TM_CCOEFF;

    /// Create the result matrix
    int result_cols = img.cols - templ.cols + 1;
    int result_rows = img.rows - templ.rows + 1;
    Mat result;
    result.create(result_rows, result_cols, CV_32FC1);

    /// Do the Matching but no Normalize
    matchTemplate(img, templ, result, match_method);

    /// Localizing the best match with minMaxLoc
    double minVal, maxVal;
    minMaxLoc(result, &minVal, &maxVal, nullptr, nullptr, Mat());

    // For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
    // double score;
    // if (match_method == cv::TemplateMatchModes::TM_SQDIFF ||
    //     match_method == cv::TemplateMatchModes::TM_SQDIFF_NORMED) {
    //     score = minVal;
    // } else {
    //     score = maxVal;
    // }
    return maxVal;
}

inline string curTimeStr() {
    std::time_t currenttime = std::time(0);
    char tAll[255] = {};
    std::strftime(tAll, sizeof(tAll), "%Y-%m-%d %H:%M:%S", std::localtime(&currenttime));
    return tAll;
}

void detectPool() {
    VideoCapture capture;
    capture.setExceptionMode(true);

    // capture.open("foo.22_06_14_20_58_32.flv");
    const string source = "rtmp://localhost:1935/live";
    printf("Opening source:%s\n", source.c_str());
    if (!capture.open(source)) {
        printf("Open stream: %s failed!\n", source.c_str());
        return;
    }

    const int IDLE_FRAMES = 60 * 60;
    // make sure lastI not cached
    volatile int lastI = -IDLE_FRAMES;

    printf("Reading template\n");
    Mat templ = cv::imread("symbol.png");
    {
        ThreadPool pool(4);
        printf("Pool Created\n");
        constexpr int framesPer = 3;
        printf("Detection started, per %d frames\n", framesPer);
        for (int i = 0;; i++) {
            if (!capture.grab()) {
                printf("No frame can grab, exiting.\n");
                break;
            }
            if (i % framesPer == 0 && i - lastI >= IDLE_FRAMES) {
                Mat image;
                capture.retrieve(image);
                Mat croppedImg = image(cv::Range(330, 800), cv::Range(500, 1450));
                pool.enqueue([i, &lastI, croppedImg = std::move(croppedImg), &templ]() {
                    if (i % (30 * 60) == 0)
                        printf("#%d\n", i);
                    double score = scoreMatching(croppedImg, templ);
                    if (score >= 0.75 && i - lastI >= IDLE_FRAMES) {
                        lastI = i;
                        int m = floor(i / 60.0 / 60.0);
                        double sec = i / 60.0 - m * 60;
                        printf("Score: %.3f, at %s\x7\n", score, curTimeStr().c_str());
                        cv::imwrite("foo.gotcha.png", croppedImg);
                    }
                });
            }
        }
    }
}

void detectSingleThread() {
    VideoCapture capture;
    capture.setExceptionMode(true);
    capture.open("foo.22_06_14_20_58_32.flv");

    // once we find one, we won't check until 1min after
    constexpr int TIME_IDLE = 60 * 60;
    int lastI = -TIME_IDLE;
    Mat templ = cv::imread("symbol.png");
    for (int i = 1;; i++) {
        if (!capture.grab()) {
            printf("No frame can grab, exiting.\n");
            break;
        }
        Mat image;
        // every 10 frame
        if (i % 6 == 0 && i - lastI > TIME_IDLE) {
            capture.retrieve(image);
            // cropped to center area
            Mat croppedImg = image(cv::Range(330, 800), cv::Range(500, 1450));
            auto msStart = nowInMs();
            double score = scoreMatching(croppedImg, templ);
            int64_t msCost = nowInMs() - msStart;
            printf("No.%d, cost: %ldms\n", i, msCost);
            if (score >= 0.75 && i - lastI > 60) {
                int m = floor(i / 60.0 / 60.0);
                double sec = i / 60.0 - m * 60;
                printf("NO. %d, Score: %.3f, at %d:%.3f\x7\n", i, score, m, sec);
                lastI = i;
            }
        }
    }
}

int main(int argc, char const *argv[]) {
    // detectSingleThread();
    detectPool();
    return 0;
}
