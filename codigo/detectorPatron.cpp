#include <iostream> // for standard I/O

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui/highgui.hpp>  // Video write
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv;


bool posiblePatron(Mat img)
{
	int canBlanco=0;
	for(int y=0; y<img.rows; y++)
	{
		for(int x=0; x<img.cols; x++)
		{
			if(img.at<uchar>(y, x)==255)
				canBlanco++;
		}
	}

	int porcentaje = canBlanco*100/(img.rows*img.cols) ;
	if(porcentaje>60 && porcentaje<99)
		return true;
	return false;
}

int main()
{
	VideoCapture cap("calibration_camara_blanca.avi");

	if(!cap.isOpened())
		return -1;

	namedWindow("webcam");

	int umbral=150;
	Mat img, imgGray, imgMod;

	cap >> img;
	int anchoImg=img.cols, altoImg=img.rows;
	int cantAncho=8, cantAlto=8;
	int anchoParte=anchoImg/cantAncho, altoParte=altoImg/cantAlto;


	string nombre;
	Mat image_roi;
	Mat image_roi_Color;
	Rect rec;
	cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(2.0, Size(2,2));
	vector<Vec3f> circles;

	for(;;)
	{
		cap >> img;

		cvtColor( img, imgGray, CV_BGR2GRAY );
		//clahe->apply(image_roi, image_roi);
		equalizeHist( imgGray, imgMod );
		blur( imgMod, imgMod, Size(5,5) );


		//for(int y=0; y<img.rows; y++)
		{
			//for(int x=0; x<img.cols; x++)
			{
				//if(imgMod.at<uchar>(y,x)>umbral)
				//imgMod.at<uchar>(y,x)=255;
				//else
				//imgMod.at<uchar>(y,x)=0;
			}
		}

		threshold(imgMod, imgMod,0,255,cv::THRESH_BINARY+cv::THRESH_OTSU);


		for(int j=0; j<cantAlto; j++)
		{
			for(int i=0; i<cantAncho; i++)
			{
				rec = Rect(i*anchoParte, j*altoParte, anchoParte , altoParte);
				image_roi = imgMod(rec);


				if(posiblePatron(image_roi))
				{

					//equalizeHist(image_roi, image_roi);
					//Canny( image_roi, image_roi, 3, 100, 3 );
					//image_roi.copyTo(imgGray(rec));
					HoughCircles( image_roi, circles, CV_HOUGH_GRADIENT, 1, 30, 3, 10, 0, 0 );
					if(circles.size()>=2)
					{
						image_roi_Color = img(rec);
						for( size_t i = 0; i < 1; i++ )
						{
							Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
							int radius = cvRound(circles[i][2]);
							// circle center
							circle( image_roi_Color, center, 2, Scalar(0,255,0), -1, 8, 0 );
							// circle outline
							circle( image_roi_Color, center, radius, Scalar(0,0,255), 2, 8, 0 );
						}

						image_roi_Color.copyTo(img(rec));
					}
				}
			}
		}
		imshow( "binAdap", img );


		if(waitKey(40) >= 0) break;
	}

	return 0;
}



