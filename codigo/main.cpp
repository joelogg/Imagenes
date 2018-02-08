#include <iostream> // for standard I/O
#include <string>
#include <math.h>

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui/highgui.hpp>  // Video write


#include "opencv2/imgproc/imgproc.hpp"


using namespace cv;
using namespace std;



Mat adaptiveThreshold(Mat in ,int w, int h)
{
	Mat out = in.clone();
	int intImg[w][h];
	for (int i = 0; i<w; i++)
	{
		for (int j = 0; j<h; j++)
		{
			intImg[i][j]=0;
		}
	}

	for (int i = 0; i<w; i++)
	{
		int sum = 0;
		for (int j = 0; j<h; j++)
		{
			sum = sum + in.at<uchar>(j,i);
			if (i == 0)
			{
				intImg[i][j] = sum;
			}
			else
			{
				intImg[i][j] = intImg[i-1][j] + sum;
			}
		}
	}

	float s=8;
	int x1, x2, y1, y2;
	float count, sum;
	float t=0.3;

	for (int i = 0; i<w; i++)
	{
		for (int j = 0; j<h; j++)
		{
			x1 = i - s/2; //{border checking is not shown}
			x2 = i + s/2;
			y1 = j - s/2;
			y2 = j + s/2;
			count = (x2-x1)*(y2-y1);
			sum = intImg[x2][y2] - intImg[x2][y1-1]-intImg[x1-1][y2] +intImg[x1-1][y1-1];

			if ( (in.at<uchar>(j,i)*count) <= (sum*(100-t)/100) )
			{
				out.at<uchar>(j,i) = 0;
			}
			else
			{
				out.at<uchar>(j,i) = 255;
			}
		}
	}

	return out;
}

void integralProyectivaX(Mat img)
{
	int altoHist = 200;
	Mat imgHist = Mat(altoHist, img.cols, CV_8UC1);
	for (int y = 0; y<altoHist; y++)
	{
		for (int x = 0; x<img.cols; x++)
		{
			imgHist.at<uchar>(y,x) = 0;
		}
	}

	int hist[img.cols];
	for(int i=0; i<img.cols; i++)
	{
		hist[i]=0;
	}

	for (int x = 0; x<img.cols; x++)
	{
		for (int y = 0; y<img.rows; y++)
		{
			if(img.at<uchar>(y,x)==255)
				hist[x] += 1;
		}
	}

	int max=0;
	for(int i=0; i<img.cols; i++)
	{
		if(max<hist[i])
			max=hist[i];
	}


	int valAlto;
	for(int x=0; x<img.cols; x++)
	{

		valAlto = altoHist * hist[x]/max ;

		//cout<<endl
		for(int y=0; y<valAlto; y++)
		{
			imgHist.at<uchar>(altoHist-y-1,x)=255;
		}
	}


	imshow( "HistoX", imgHist );
}

void integralProyectivaY(Mat img)
{
	int anchoHist = 200;
	Mat imgHist = Mat(img.rows, anchoHist, CV_8UC1);
	for (int y = 0; y<img.rows; y++)
	{
		for (int x = 0; x<anchoHist; x++)
		{
			imgHist.at<uchar>(y,x) = 0;
		}
	}

	int hist[img.rows];
	for(int i=0; i<img.rows; i++)
	{
		hist[i]=0;
	}

	for (int y = 0; y<img.rows; y++)
	{
		for (int x = 0; x<img.cols; x++)
		{
			if(img.at<uchar>(y,x)==255)
				hist[y] += 1;
		}
	}

	int max=0;
	for(int i=0; i<img.rows; i++)
	{
		if(max<hist[i])
			max=hist[i];
	}


	int valAlto;
	for(int i=0; i<img.rows; i++)
	{
		valAlto = anchoHist * hist[i]/max ;

		for(int x=0; x<valAlto; x++)
		{
			imgHist.at<uchar>(i, x)=255;
		}
	}


	imshow( "HistoY", imgHist );
}

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
	//cout<<porcentaje<<endl;
	if(porcentaje>60 && porcentaje<99)
		return true;
	return false;
}
struct Circulo
{
	int x;
	int y;
	int r;
public:
	Circulo(){};
	Circulo(int x, int y, int r)
{
		this->x = x;
		this->y = y;
		this->r = r;
}
};


int main()
{
	Mat imgO, imgO2, img, imgMod;
	imgO = imread( "frame.png", 1 );
	cvtColor( imgO, img, CV_BGR2GRAY );

	imgO2 = imgO.clone();
	imgMod = img.clone();

	cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(2.0, Size(2,2));


	//clahe->apply(imgMod, imgMod);
	equalizeHist( imgMod, imgMod );
	blur( imgMod, imgMod, Size(5,5) );
	threshold(imgMod, imgMod,0,255,cv::THRESH_BINARY+cv::THRESH_OTSU);
	
	//Canny( imgMod, imgMod, 3, 100, 3 );
	//for(int y=0; y<img.rows; y++)
	{
		//for(int x=0; x<img.cols; x++)
		{
			//if(imgMod.at<uchar>(y,x)>150)
			//imgMod.at<uchar>(y,x)=255;
			//else
			//imgMod.at<uchar>(y,x)=0;
		}
	}


	//imshow( "binarioYo", imgMod );

	int anchoImg=img.cols, altoImg=img.rows;
	int cantAncho=8, cantAlto=8;
	int anchoParte=anchoImg/cantAncho, altoParte=altoImg/cantAlto;

	string nombre;
	Mat image_roi;
	Mat image_roi_Color;
	Rect rec;
	vector<Vec3f> circles;
	vector<Circulo> circlesTotal;


	for(int j=0; j<cantAlto; j++)
	{
		for(int i=0; i<cantAncho; i++)
		{
			rec = Rect(i*anchoParte, j*altoParte, anchoParte , altoParte);
			image_roi = imgMod(rec);


			nombre = "Roid:"+ to_string(j) + "-" + to_string(i);
			//cout<<nombre<<endl;
			rectangle(imgO, rec, CV_RGB(0,255,255));


			if(posiblePatron(image_roi))
			{
				//5 ultimos (Separacion entre centros, umbral canny , Umbral detección centro , minRadio, maxRadio)
				HoughCircles( image_roi, circles, CV_HOUGH_GRADIENT, 1, 1, 10, 15, 0, 0 );
				cout<<"Tam: "<<circles.size()<<endl;
				if(circles.size()>=2)
				{
					image_roi_Color = imgO(rec);
					for( size_t k = 0; k < circles.size(); k++ )
					{
						circlesTotal.push_back(Circulo(circles[k][0]+i*anchoParte, circles[k][1]+j*altoParte, circles[k][2]));
					}
				}

			}



		}
	}


	cout<<"Total: "<<circlesTotal.size()<<endl;
	int radProm=0;
	for(int i=0; i<circlesTotal.size(); i++)
	{
		cout<<"RadioA: "<< circlesTotal[i].r <<endl;
		radProm += circlesTotal[i].r;
	}
	radProm /= circlesTotal.size();

	cout<<"Radio: "<<radProm<<endl;


	for(int i=0; i<circlesTotal.size(); i++)
	{
		Point center(circlesTotal[i].x, circlesTotal[i].y);
		int radius = radProm;
		circle( imgO2, center, 2, Scalar(0,255,0), -1, 8, 0 );
		// circle outline
		circle( imgO2, center, radius, Scalar(0,0,255), 2, 8, 0 );
	}

	vector<Circulo> patron;
	for(int i=0; i<20; i++)
	{
		patron.push_back(Circulo(0, 0, radProm));
	}


	Circulo cIni, cSig;
	int dis, disMax;
	for(int i=0; i<circlesTotal.size(); i++)
	{
		cIni =  circlesTotal[i];
		for(int j=i+1; j<circlesTotal.size(); j++)
		{
			cSig =  circlesTotal[j];
			dis = sqrt( pow(cSig.x-cIni.x,2) + pow(cSig.y-cIni.y,2));
			disMax = sqrt( pow(radProm,2) + pow(radProm,2));
			if(radProm-3<dis && dis<disMax+3)
			{
				patron[0].x = cSig.x;
				patron[0].y = cSig.y;
				patron[0].r = cSig.r;
				i=circlesTotal.size();
				break;
			}

		}
	}

	for(int i=0; i<20; i++)
	{
		Point center(patron[i].x, patron[i].y);
		int radius = patron[i].r;
		circle( imgO, center, 2, Scalar(0,255,0), -1, 8, 0 );
		circle( imgO, center, radius, Scalar(0,0,255), 2, 8, 0 );
	}




	imshow( "Original", imgO );
	imshow( "Original2", imgO2 );
	//imshow( "OriginalGro", imgMod );




	waitKey(0);







	return 0;
}

