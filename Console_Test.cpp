/*
* Done by:
    - RADOUANI Nadia. (MLDS 21911973)
    - KOUIDER Amira. (MLDS 21904040)
* Project: Stairs detection & steps counter
*/
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include"opencv2/highgui/highgui.hpp"
#include <opencv2/imgproc/imgproc_c.h>

using namespace std;
using namespace cv;

int main() {
    //-------- DECLARATION --------------
    // --------- IMAGES
    Mat Image, ImageGRAY, ImageLAPLACE, ImageSOBEL, ImageCROPED, Crop, ImageLINES;
    // ---------- TO CROP
    double a = -1, b = -1, aa, bb, sizeA, sizeB;
    // ----------- LINES
    std::vector<cv::Vec4i> lines;
    std::vector<Point> point;
    std::vector<Point> point2;
    // ---------- Order of Sobel derivation
    Mat Sobel_x, AbsSobel_x, Sobel_y, AbsSobel_y;

    // -------------- IMPORT ----------------
    Image = imread("20.jpg");
    if (!Image.data) {
        cout << "Could not open or find the image" << endl;
        return -1;
    }

    // ------------- CROP & RESIZE ---------------
    // ----- TO USE IN CASE THE IMAGE IS TOO BIG 
    if (Image.cols > 1000 && Image.rows > 1000) {
        Rect myROI(100, 100, Image.cols - 100, Image.rows - 100);
        Image = Image(myROI);
    }
    // FOR ROWS
    for (int i = 0; i < Image.rows; i++) {
        for (int j = 0; j < Image.cols; j++) {
            if (Image.at<uchar>(i, j) >= 0 && Image.at<uchar>(i, j) < 80) {

                if (b == -1) {
                    b = i;
                }
                bb = i;
                break;
            }
        }
    }
    // FOR COLUMNS
    for (int i = 0; i < Image.cols; i++) {
        for (int j = 0; j < Image.rows; j++) {
            if (Image.at<uchar>(j, i) >= 0 && Image.at<uchar>(j, i) < 80) {
                if (a == -1) {
                    a = i;
                }
                aa = i;
                break;
            }
        }
    }

    sizeA = aa - a;
    sizeB = bb - b;

    Rect myROI(a, b, sizeA, sizeB);
    Crop = Image(myROI);

    resize(Crop, ImageCROPED, Size(500, 500));
    imshow("Croped Image", ImageCROPED);

    //-------------- APPLY GAUSSIAN BLUR -----------
    GaussianBlur(ImageCROPED, ImageCROPED, Size(3, 3), 0, 0, BORDER_DEFAULT);
    imshow("Blur Image", ImageCROPED);


    // ------------- CONVERT TO GRAY ----------
    cvtColor(ImageCROPED, ImageGRAY, COLOR_RGB2GRAY, 0);
    imshow("Image gray", ImageGRAY);


    // ------------- DETECT CONTOURS -----------
    // -------- USING LAPLACE
    Laplacian(ImageGRAY, ImageLAPLACE, CV_16S, 3);
    convertScaleAbs(ImageLAPLACE, ImageLAPLACE);
    imshow("Image LAPLACE", ImageLAPLACE);

    // -------- USING SOBEL
    // In x direction
    Sobel(ImageGRAY, Sobel_x, CV_16S, 1, 0, 3, 1, 0, BORDER_DEFAULT);
    convertScaleAbs(Sobel_x, AbsSobel_x);
    // In y direction
    Sobel(ImageGRAY, Sobel_y, CV_16S, 0, 1, 3, 1, 0, BORDER_DEFAULT);
    convertScaleAbs(Sobel_y, AbsSobel_y);
    // Total
    addWeighted(AbsSobel_x, 0.5, AbsSobel_y, 0.5, 0, ImageSOBEL);
    imshow("Image SOBEL ", ImageSOBEL);

    //Since we get better results using edge detection Sobel, we will continue the rest of the program using ImageSOBEL

    //---------- DETECTION & DRAWING LINES ----------------
    // -------------- DETECTION
    cv::threshold(ImageSOBEL, ImageLINES, 0, 255, THRESH_BINARY | THRESH_OTSU);
    HoughLinesP(ImageLINES, lines, 1, CV_PI / 180, 50, 180, 10);
    imshow("Hough Lines", ImageLINES);

    // --------------- DRAW LINES 
    Mat img = cv::Mat::zeros(ImageLINES.rows, ImageLINES.cols, CV_8UC1);
    for (size_t i = 0; i < lines.size(); i++) {
        Vec4i l = lines[i];
        if (0 < abs(l[3] - l[1]) / abs(l[2] - l[0]) < 1) {
            line(img, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 0, 0), 3, LINE_AA);
            point.push_back(Point(l[0], l[1]));
        }
    }
    imshow("Result Image", img);

    // -------------- DETECTION OF CLOSE POINTS TO x AND FAR FROM y IN ORDER TO DETECT THE CORNER OF STAIRS
    int x = 5, y = 8;
    for (int i = 0; i < point.size(); i++) {
        if (i > 0)
            if (abs(point[i].x - point[i - 1].x) < x || abs(point[i].y - point[i - 1].y) < y)
                point.erase(point.begin() + i);
    }
    point2 = point;

    std::cout << "Number of stairs' steps: " << point2.size() << "\n";

    waitKey();
    return 0;
}