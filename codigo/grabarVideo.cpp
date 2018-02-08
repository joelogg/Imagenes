/*#include <ctime>
#include <iostream> // for standard I/O
#include <string>   // for strings

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui/highgui.hpp>  // Video write

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
	time_t o;
	o = time(NULL);



	VideoCapture inputVideo(0);
	if (!inputVideo.isOpened())
	{
		cout  << "No lee camara " << endl;
		return -1;
	}

	Mat src;
	inputVideo >> src;

	string nombre = string(asctime(localtime(&o)))+".avi";
	VideoWriter outputVideo;
	outputVideo.open(nombre, CV_FOURCC('X','V','I','D'), 30, Size(src.cols,src.rows), true);
	if (!outputVideo.isOpened())
	{
		cout  << "No puede abrir captura de salida " << endl;
		return -1;
	}

	for(;;)
	{
		inputVideo >> src;
		if (src.empty()) break;
		imshow("webcam", src);

		//outputVideo.write(src); //save or
		outputVideo << src;
		 if(waitKey(1) >= 0) break;
	}

	cout << "Finished writing" << endl;
	return 0;
}
*/
