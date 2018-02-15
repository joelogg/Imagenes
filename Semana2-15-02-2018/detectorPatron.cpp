#include <iostream> // for standard I/O

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui/highgui.hpp>  // Video write
#include "opencv2/imgproc/imgproc.hpp"


#include "opencv2/core/core_c.h"
#include "opencv2/core/core.hpp"
#include "opencv2/flann/miniflann.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

struct Anillo
{
public:
	int id=-1;//no alineado
	int cx=0;
	int cy=0;
	int xI=0;
	int yI=0;
	int anchoInterior=0;
	int altoInterior=0;
	int radioInterior=0;
	int anchoExterior=0;
	int altoExterior=0;
	int radioExterior=0;



	Anillo()
	{}

	Anillo(int x, int y, int anchoInterior, int altoInterior, int anchoExterior, int altoExterior)
	{
		this->cx = x;
		this->cy = y;
		this->xI = x - anchoExterior/2;
		this->yI = y - altoExterior/2;
		this->anchoInterior = anchoInterior;
		this->altoInterior = altoInterior;
		this->radioInterior = (anchoInterior+altoInterior)/4;
		this->anchoExterior = anchoExterior;
		this->altoExterior = altoExterior;
		this->radioExterior = (anchoExterior+altoExterior)/4;
	}
	void setAltoExterior(int aumentoAlto)
	{
		this->altoExterior = this->altoExterior + aumentoAlto;
		this->radioExterior = (anchoExterior+altoExterior)/4;
		this->cy = this->cy - altoExterior/2;
	}
	void setAnchoExterior(int aumentoAncho)
	{
		this->anchoExterior = this->anchoExterior + aumentoAncho;
		this->radioExterior = (anchoExterior+altoExterior)/4;
		this->cx = this->cx - anchoExterior/2;
	}
	void setXI(int nXI)
	{
		this->xI = nXI;
		this->cx = nXI + anchoExterior/2;
	}
	void setYI(int nYI)
	{
		this->yI = nYI;
		this->cy = nYI + altoExterior/2;
	}
};

struct MaxMin
{
public:
	int maxX=0;
	int minX=0;
	int maxY=0;
	int minY=0;
	MaxMin() {}
	MaxMin(int minX, int maxX, int minY, int maxY)
	{
		this->maxX = maxX;
		this->minX = minX;
		this->maxY = maxY;
		this->minY = minY;
	}
};

int vecino(int pos, vector<Anillo> patron)
{
	int posMin=0;
	int disMin=300;
	Point p1(patron[pos].cx, patron[pos].cy);
	Point p2;

	for (int i = patron.size()-1;  i >=0; i--)
	{
		p2 = Point(patron[i].cx, patron[i].cy);
		if(patron[i].id==-1 && disMin> sqrt(pow( (p1.x-p2.x) ,2)+pow( (p1.y-p2.y)  ,2)))
		{
			posMin = i;
			disMin = sqrt(pow( (p1.x-p2.x) ,2)+pow( (p1.y-p2.y)  ,2));
		}
	}
	return posMin;
}

MaxMin buscarArriba(Mat imgBin, int x, int y, MaxMin valores, int colorSemilla);
MaxMin buscarAbajo(Mat imgBin, int x, int y, MaxMin valores, int colorSemilla);

MaxMin buscarIzquierda(Mat imgBin, int x, int y, MaxMin valores, int colorSemilla)
{
	if(x>=0 && y>=0 && x<imgBin.cols && y<imgBin.rows && imgBin.at<uchar>(y,x)==colorSemilla)
	{
		imgBin.at<uchar>(y,x) = 127;
		if(valores.minX > x)
			valores.minX = x;
		valores =  buscarIzquierda(imgBin, x-1, y, valores, colorSemilla);
		valores =  buscarAbajo(imgBin, x, y+1, valores, colorSemilla);
		valores = buscarArriba(imgBin, x, y-1, valores, colorSemilla);
	}
	return valores;
}

MaxMin buscarDerecha(Mat imgBin, int x, int y, MaxMin valores, int colorSemilla)
{
	if(x>=0 && y>=0 && x<imgBin.cols && y<imgBin.rows && imgBin.at<uchar>(y,x)==colorSemilla)
	{
		imgBin.at<uchar>(y,x) = 127;
		if(valores.maxX < x)
			valores.maxX = x;
		valores =  buscarDerecha(imgBin, x+1, y, valores, colorSemilla);
		//valores =  buscarAbajo(imgBin, x, y+1, valores, colorSemilla);
		//valores = buscarArriba(imgBin, x, y-1, valores, colorSemilla);

	}
	return valores;
}

MaxMin buscarAbajo(Mat imgBin, int x, int y, MaxMin valores, int colorSemilla)
{
	if(x>=0 && y>=0 && x<imgBin.cols && y<imgBin.rows && imgBin.at<uchar>(y,x)==colorSemilla)
	{
		imgBin.at<uchar>(y,x) = 127;
		if(valores.maxY < y)
			valores.maxY = y;
		valores =  buscarAbajo(imgBin, x, y+1, valores, colorSemilla);
		valores =  buscarDerecha(imgBin, x+1, y, valores, colorSemilla);
		valores =  buscarIzquierda(imgBin, x-1, y, valores, colorSemilla);

	}
	return valores;
}

MaxMin buscarArriba(Mat imgBin, int x, int y, MaxMin valores, int colorSemilla)
{
	if(x>=0 && y>=0 && x<imgBin.cols && y<imgBin.rows && imgBin.at<uchar>(y,x)==colorSemilla)
	{
		imgBin.at<uchar>(y,x) = 127;
		if(valores.minY > y)
			valores.minY = y;
		valores = buscarArriba(imgBin, x, y-1, valores, colorSemilla);
		valores =  buscarDerecha(imgBin, x+1, y, valores, colorSemilla);
		valores =  buscarIzquierda(imgBin, x-1, y, valores, colorSemilla);

	}
	return valores;
}



Rect encontrarPatron(Mat imgBin, int x, int y)
{

	Rect rec;
	int colorSemilla = (int)imgBin.at<uchar>(y,x);
	imgBin.at<uchar>(y,x) = 127;
	MaxMin valores(x, x, y, y);
	if( !(colorSemilla==0 || colorSemilla==255) )
	{
		rec = Rect(valores.minX, valores.minY, (valores.maxX-valores.minX), (valores.maxY-valores.minY));
		return rec;
	}

	valores =  buscarArriba(imgBin, x, y-1, valores, colorSemilla);
	valores =   buscarAbajo(imgBin, x, y+1, valores, colorSemilla);
	valores = buscarDerecha(imgBin, x+1, y, valores, colorSemilla);
	valores = buscarIzquierda(imgBin, x-1, y, valores, colorSemilla);

	rec = Rect(valores.minX, valores.minY, (valores.maxX-valores.minX), (valores.maxY-valores.minY));
	return rec;
}

Rect encontrarPatron2(Mat imgBin, int x, int y, int ancho, int alto)
{

	Rect rec;
	int colorSemilla;
	MaxMin valores;

	//buscar arriba
	colorSemilla = (int)imgBin.at<uchar>(y-1.5*alto/2,x);
	valores = MaxMin(x, x, y, y);
	if( !(colorSemilla==0 || colorSemilla==255) )
	{
		rec = Rect(valores.minX, valores.minY, (valores.maxX-valores.minX), (valores.maxY-valores.minY));
		return rec;
	}
	valores =  buscarArriba(imgBin, x, y-1.5*alto/2, valores, colorSemilla);

	colorSemilla = (int)imgBin.at<uchar>(y+1.5*alto/2,x);
	if( !(colorSemilla==0 || colorSemilla==255) )
	{
		rec = Rect(valores.minX, valores.minY, (valores.maxX-valores.minX), (valores.maxY-valores.minY));
		return rec;
	}
	valores =  buscarAbajo(imgBin, x, y+1.5*alto/2, valores, colorSemilla);

	colorSemilla = (int)imgBin.at<uchar>(y,x+1.5*ancho/2);
	if( !(colorSemilla==0 || colorSemilla==255) )
	{
		rec = Rect(valores.minX, valores.minY, (valores.maxX-valores.minX), (valores.maxY-valores.minY));
		return rec;
	}
	valores = buscarDerecha(imgBin, x+1.5*ancho/2, y, valores, colorSemilla);

	colorSemilla = (int)imgBin.at<uchar>(y,x-1.5*ancho/2);
	if( !(colorSemilla==0 || colorSemilla==255) )
	{
		rec = Rect(valores.minX, valores.minY, (valores.maxX-valores.minX), (valores.maxY-valores.minY));
		return rec;
	}
	valores = buscarIzquierda(imgBin, x-1.5*ancho/2, y, valores, colorSemilla);

	rec = Rect(valores.minX, valores.minY, (valores.maxX-valores.minX), (valores.maxY-valores.minY));
	return rec;
}

int buscarEspacioArriba(Mat roi);
int buscarEspacioAbajo(Mat roi);
int buscarEspacioIzquierda(Mat roi);
int buscarEspacioDerecha(Mat roi);

Point encontrarPuntoId(vector<Anillo> patronAnillos, int pos);

int main()
{
	VideoCapture cap("2018/calibration_ps3eyecam.avi");
	//VideoCapture cap("2018/calibration_mslifecam.avi");
	//VideoCapture cap("2018/realsense_RGB.avi");

	//VideoCapture cap("2017/PadronAnillos_01.avi");
	//VideoCapture cap("2017/PadronCirculos_01.avi");

	if(!cap.isOpened())
		return -1;

	SimpleBlobDetector::Params params;
	// Filter by Convexity
	params.filterByConvexity = true;
	params.minConvexity = 0.8;
	// Filter by Inertia
	params.filterByInertia = true;
	params.minInertiaRatio = 0.4;
	Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
	vector<KeyPoint> keypoints;
	vector<Anillo> patron;
	vector<Anillo> patronAnillos;

	Mat imgO, imgGris, imgBlur, imgCanny, imgBinaria, imgDilEr, imgDilEr2;
	Mat roi;
	Mat imgFinal;

	Rect rec, rec2;
	int cx1, cx2, cy1, cy2;

	int key;

	bool patronEncontrado = false;
	//int errores = 0;

	for(;;)
	{
		cap >> imgO;

		cvtColor( imgO, imgGris, CV_BGR2GRAY );
		blur( imgGris, imgBlur, Size(5,5) );
		adaptiveThreshold(imgBlur, imgBinaria,255,ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV,11,3);
		int dilation_size = 2;
		Mat element = getStructuringElement( MORPH_ELLIPSE, Size( 2*dilation_size + 1, 2*dilation_size+1 ), Point( dilation_size, dilation_size ) );
		dilate(imgBinaria, imgDilEr, element);
		erode(imgDilEr, imgDilEr, element);
		imgDilEr2 = imgDilEr.clone();

		Canny( imgDilEr, imgCanny, 3, 100, 3 );
		//Canny( imgGris, imgCanny, 3, 100, 3 );
		imgFinal = imgO.clone();
		if(patronEncontrado==false)
		{
			detector->detect( imgCanny, keypoints);

			patron.clear();
			patronAnillos.clear();

			//Primer Centro
			for (size_t i = 0; i < keypoints.size() && keypoints.size()>15; ++i)
			{
				rec = encontrarPatron(imgDilEr, keypoints[i].pt.x, keypoints[i].pt.y);

				if(rec.x>2 && rec.y>2 && rec.width>6 && rec.height>6 &&
						rec.width<imgDilEr.cols/8 && rec.height<imgBinaria.cols/8 &&
						abs(rec.width-rec.height)<=10)
				{
					//rectangle(imgFinal, rec, CV_RGB(0,255,255));
					cx1 = rec.x + rec.width/2;
					cy1 = rec.y + rec.height/2;
					patron.push_back( Anillo(cx1, cy1, rec.width, rec.height, rec.width, rec.height) );
				}

			}

			//-------------------Eliminando centros no relacionados----------------
			int tam = patron.size()-1;
			bool proximo;
			if(patron.size()>0)
			{
				for(int i=tam; i>0; i--)
				{
					Anillo anillo = patron[i];
					proximo = false;
					for(int j=patron.size()-1; j>=0; j--)
					{
						if( j!=i && sqrt( pow(anillo.cx-patron[j].cx,2)+pow(anillo.cy-patron[j].cy,2) ) <  anillo.radioInterior*8 &&
								sqrt( pow(anillo.cx-patron[j].cx,2)+pow(anillo.cy-patron[j].cy,2) ) >  anillo.radioInterior*4)
						{
							proximo = true;
							break;
						}
					}
					if(proximo==false)
					{
						patron.erase(patron.begin()+i);
					}
				}
			}

			//Para el ultimo
			if(patron.size()>0)
			{
				Anillo anillo = patron[0];
				proximo = false;
				for(size_t j=1 ; j<patron.size(); j++)
				{
					if( sqrt( pow(anillo.cx-patron[j].cx,2)+pow(anillo.cy-patron[j].cy,2) ) <  anillo.radioInterior*8 &&
							sqrt( pow(anillo.cx-patron[j].cx,2)+pow(anillo.cy-patron[j].cy,2) ) >  anillo.radioInterior*4)
					{
						proximo = true;
						break;
					}
				}
				if(proximo==false)
				{
					patron.erase(patron.begin());
				}
			}

			//Segundo Centro
			if(patron.size()>=15)
			{
				for (size_t i = 0; i < patron.size(); ++i)
				{
					rec2 = encontrarPatron2(imgDilEr2, patron[i].cx, patron[i].cy, patron[i].anchoInterior, patron[i].altoInterior);

					cx1 = patron[i].cx;
					cy1 = patron[i].cy;
					cx2 = rec2.x + rec2.width/2;
					cy2 = rec2.y + rec2.height/2;

					rec = Rect(patron[i].cx-patron[i].anchoExterior, patron[i].cy-patron[i].altoInterior, patron[i].altoInterior, patron[i].altoInterior);

					//rectangle(imgFinal, rec2, CV_RGB(0,255,255));
					if(sqrt( pow(cx1-cx2,2) + pow(cy1-cy2,2) )<=4 && rec.width>2 && rec.height>2 && rec2.width>2 && rec2.height>2)
					{
						patronAnillos.push_back( Anillo( (cx1+cx2)/2, (cy1+cy2)/2, rec.width, rec.height, rec2.width, rec2.height) );
					}
				}
			}


			for(size_t i=0; i<patronAnillos.size(); i++)
			{
				circle(imgFinal, Point(patronAnillos[i].cx, patronAnillos[i].cy), 2, Scalar(255, 0, 255), -1);
			}


			if(patronAnillos.size()==20)
			{
				int posA = patronAnillos.size()-1, posV;
				int cant=1;
				int pos5 = patronAnillos.size()-1;
				patronAnillos[posA].id = cant;
				Point p1 (patronAnillos[posA].cx, patronAnillos[posA].cy);
				Scalar color = Scalar(225, 0, 0);
				circle(imgFinal, p1, patronAnillos[posA].radioInterior, color, 1);
				Point p2;

				while(cant<20)
				{
					if(cant%5==0)
						posV = vecino(pos5, patronAnillos);
					else
						posV = vecino(posA, patronAnillos);

					if(cant%20==0)
						color = Scalar(255, 225, 0);
					else if(cant%15==0)
						color = Scalar(0, 225, 0);
					else if(cant%10==0)
						color = Scalar(0, 255, 255);
					else if(cant%5==0)
						color = Scalar(0, 0, 255);

					p2 = Point(patronAnillos[posV].cx, patronAnillos[posV].cy);
					circle(imgFinal, p2, patronAnillos[posV].radioInterior, color, 1);
					//circle(drawImage, p2, patron[i].radioExterior, Scalar(255, 0, 255), 1);
					line( imgFinal, p1, p2, color,  2, 8 );
					//rectangle(imgFinal, Rect(patronAnillos[posV].xI, patronAnillos[posV].yI, patronAnillos[posV].anchoExterior, patronAnillos[posV].altoExterior), CV_RGB(0,255,255));
					//rectangle(drawImage, Rect(patron[i].x-patron[i].anchoExterior/2, patron[i].y-patron[i].altoExterior/2, patron[i].anchoExterior, patron[i].altoExterior), CV_RGB(0,255,255));

					cant++;
					p1 = Point(patronAnillos[posV].cx, patronAnillos[posV].cy);
					patronAnillos[posV].id = cant;

					posA = posV;
				}
				patronEncontrado = true;
			}
		}
		else
		{
			//cout<<"----------------"<<endl;			
			for(int i=patronAnillos.size()-1; i>=0; i--)
			//for(int i=patronAnillos.size()-1; i>=patronAnillos.size()-1; i--)
			{
				roi = imgDilEr2(Rect(patronAnillos[i].xI, patronAnillos[i].yI, patronAnillos[i].anchoExterior, patronAnillos[i].altoExterior));
				rectangle(imgFinal, Rect(patronAnillos[i].xI, patronAnillos[i].yI, patronAnillos[i].anchoExterior, patronAnillos[i].altoExterior), CV_RGB(0,255,255));
				//imshow( to_string(i), roi );

				int espacioArriba = buscarEspacioArriba(roi);				
				int espacioAbajo = buscarEspacioAbajo(roi);
				//cout<<"Arriba: "<<espacioArriba<<endl;
				//cout<<"Abajo: "<<espacioAbajo<<endl;

				if(espacioArriba > 0 && espacioAbajo == 0) // hay solo espacio arriba
				{
					//cout<<"Arriba"<<endl;
					patronAnillos[i].setYI(patronAnillos[i].yI + espacioArriba);
				}
				else if((espacioArriba == 0 && espacioAbajo > 0)) // hay solo espacio abajo
				{
					//cout<<"Abajo"<<endl;
					patronAnillos[i].setYI(patronAnillos[i].yI - espacioAbajo);
				}
				else if(espacioArriba > 0 && espacioAbajo > 0) // hay espacio arriba y abajo
				{
					//cout<<"Arriba+++++++++++++++++++"<<endl;
					//cout<<"Abajo"<<endl;
					patronAnillos[i].setYI(patronAnillos[i].yI + espacioArriba);	
					patronAnillos[i].setAltoExterior(-espacioArriba - espacioAbajo);
				}
				else
				{
					//cout<<"alto mayor"<<endl;
					//patronAnillos[i].yI = patronAnillos[i].yI + espacioArriba;	
				}

				
				int espacioIzquierda = buscarEspacioIzquierda(roi);
				int espacioDerecha = buscarEspacioDerecha(roi);	
				//cout<<"Izquierda: "<<espacioIzquierda<<endl;
				//cout<<"Derecha: "<<espacioDerecha<<endl;				

				if(espacioIzquierda > 0 && espacioDerecha == 0) // hay solo espacio izquierda
				{
					//cout<<"Izquierda"<<endl;
					patronAnillos[i].setXI(patronAnillos[i].xI + espacioIzquierda);
				}
				else if((espacioIzquierda == 0 && espacioDerecha > 0)) // hay solo espacio derecha
				{
					//cout<<"Derecha"<<endl;
					patronAnillos[i].setXI(patronAnillos[i].xI - espacioDerecha);
				}
				else if(espacioIzquierda > 0 && espacioDerecha > 0) // hay espacio izquierda y derecha
				{
					//cout<<"Izquierda++++++++++++"<<endl;
					//cout<<"Derecha"<<endl;
					patronAnillos[i].setXI(patronAnillos[i].xI + espacioIzquierda);
					patronAnillos[i].setAnchoExterior(-espacioDerecha - espacioIzquierda);
				}
				else
				{
					//cout<<"ancho mayor"<<endl;
					//patronAnillos[i].xI = patronAnillos[i].xI + espacioIzquierda;
				}
				

				rectangle(imgFinal, Rect(patronAnillos[i].xI, patronAnillos[i].yI, patronAnillos[i].anchoExterior, patronAnillos[i].altoExterior), CV_RGB(255,255,0));
				//roi = imgDilEr2(Rect(patronAnillos[i].xI, patronAnillos[i].yI, patronAnillos[i].anchoExterior, patronAnillos[i].altoExterior));
				//imshow( to_string(i+20), roi );
			}

			Scalar color = Scalar(225, 0, 0);
			Point p1, p2;
			p1 = encontrarPuntoId(patronAnillos, 1);

			for(size_t i=1; i<patronAnillos.size(); i++)
			{
				if(i%15==0)
					color = Scalar(0, 225, 0);
				else if(i%10==0)
					color = Scalar(0, 255, 255);
				else if(i%5==0)
					color = Scalar(0, 0, 255);

				p2 = encontrarPuntoId(patronAnillos, i+1);
				line( imgFinal, p1, p2, color,  2, 8 );
				p1 = p2;
			}
			
			//cout<<endl;
			//waitKey(0);
		}

		imshow( "Final", imgFinal );
		//imshow( "Binaria", imgDilEr2 );

		//key = waitKey(40);
		key = waitKey(40);
		switch (key)
		{
		case 'q':
			return 0;
		case 'p':
			imwrite("frameCpturado.png",imgO);
			waitKey(0);
			break;

		}
	}

	return 0;
}

Point encontrarPuntoId(vector<Anillo> patronAnillos, int pos)
{
	for(size_t i=0; i<patronAnillos.size(); i++)
	{
		if(patronAnillos[i].id==pos)
			return Point(patronAnillos[i].cx, patronAnillos[i].cy);
	}
	return Point(50,50);
}
int buscarEspacioArriba(Mat roi)
{
	int espacioArriba = 0;
	for(int y=0; y<roi.rows; y++)
	{
		for(int x=0; x<roi.cols; x++)
		{
			if(roi.at<uchar>(y,x)==255)
			{
				espacioArriba = y;
				x = roi.rows;
				y = roi.cols;
			}
		}
	}
	return espacioArriba;
}

int buscarEspacioAbajo(Mat roi)
{
	int espacioAbajo = 0;
	for(int y=roi.rows-1; y>=0; y--)
	{
		for(int x=0; x<roi.cols; x++)
		{
			if(roi.at<uchar>(y,x)==255)
			{
				espacioAbajo = (roi.rows-1)-y;
				x = roi.rows;
				y = 0;
			}
		}
	}
	return espacioAbajo;
}

int buscarEspacioIzquierda(Mat roi)
{
	int espacioIzquierda = 0;
	for(int x=0; x<roi.cols; x++)
	{
		for(int y=0; y<roi.rows; y++)
		{
			if(roi.at<uchar>(y,x)==255)
			{
				espacioIzquierda = x;
				x = roi.rows;
				y = roi.cols;
			}
		}
	}
	return espacioIzquierda;
}

int buscarEspacioDerecha(Mat roi)
{
	int espacioDerecha = 0;
	for(int x=roi.cols-1; x>=0; x--)
	{
		for(int y=0; y<roi.rows; y++)
		{
			if(roi.at<uchar>(y,x)==255)
			{
				espacioDerecha = (roi.cols-1)-x;
				x = 0;
				y = roi.cols;
			}
		}
	}
	return espacioDerecha;
}