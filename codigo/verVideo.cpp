/*#include <iostream> // for standard I/O

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui/highgui.hpp>  // Video write

using namespace std;
using namespace cv;

int main()
{
    VideoCapture cap("calibration_camara_blanca.avi");

    if(!cap.isOpened())
        return -1;

    namedWindow("webcam");

    for(;;)
    {
        Mat frame;

        cap >> frame;

        imshow("webcam", frame);
        //imwrite( "frame.png", frame );
        //break;

        if(waitKey(30) >= 0) break;
    }

    return 0;
}
*/
