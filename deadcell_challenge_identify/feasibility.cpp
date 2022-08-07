
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/img_hash.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"

#include <chrono>
#include <unistd.h>

using cv::Mat;
using cv::Point;
using cv::Scalar;
using cv::VideoCapture;
using namespace std;

Mat performMatch(const Mat &source, const Mat &templ) {
    Mat img = source.clone();
    cv::TemplateMatchModes match_method;
    // match_method = cv::TemplateMatchModes::TM_SQDIFF;
    // match_method = cv::TemplateMatchModes::TM_CCORR;
    // match_method = cv::TemplateMatchModes::TM_CCOEFF;
    // match_method = cv::TemplateMatchModes::TM_SQDIFF_NORMED;
    // match_method = cv::TemplateMatchModes::TM_CCORR_NORMED;
    match_method = cv::TemplateMatchModes::TM_CCOEFF_NORMED;
    /// Source image to display
    Mat result;

    /// Create the result matrix
    int result_cols = img.cols - templ.cols + 1;
    int result_rows = img.rows - templ.rows + 1;

    result.create(result_rows, result_cols, CV_32FC1);

    /// Do the Matching and Normalize
    matchTemplate(img, templ, result, match_method);
    // normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, Mat());

    /// Localizing the best match with minMaxLoc
    double minVal;
    double maxVal;
    Point minLoc;
    Point maxLoc;
    Point matchLoc;

    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

    printf("min, max: %.3f %.3f; ", minVal, maxVal);

    /// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods,
    /// the higher the better

    if (match_method == cv::TemplateMatchModes::TM_SQDIFF ||
        match_method == cv::TemplateMatchModes::TM_SQDIFF_NORMED) {
        matchLoc = minLoc;
    } else {
        matchLoc = maxLoc;
    }

    printf("matchLoc x,y: %d %d\n", matchLoc.x, matchLoc.y);

    /// Show me what you got
    rectangle(img, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows),
              Scalar::all(0), 2, 8, 0);
    rectangle(result, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows),
              Scalar::all(0), 2, 8, 0);
    return img;
}

void matchTeset() {
    Mat templ = cv::imread("symbol.png");
    Mat extrue = cv::imread("example.png");
    Mat result = performMatch(extrue, templ);
    cv::imwrite("foo.res.true.png", result);
    Mat exfalse = cv::imread("foo.example.png");
    result = performMatch(exfalse, templ);
    cv::imwrite("foo.res.false.png", result);
}
