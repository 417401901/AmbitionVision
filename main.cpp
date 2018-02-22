#include "thewindow.h"
#include "opencv2/opencv.hpp"
#include "ArmorDetector.h"
//#include <QtWidgets/QApplication>
using namespace cv;
int main(int argc, char *argv[])
{
	/*QApplication a(argc, argv);
	TheWindow w;
	w.show();
	return a.exec();*/
	VideoCapture cap;
	ArmorDetector armordetctor;
	while (true)
	{
		Mat frame;
		uchar key;
		cap >> frame;
		if (!frame.empty())
		{
			resize(frame, frame, Size(1280, 720));
			armordetctor.imageSet(frame);
			imshow("สำฦต", frame);
			key = waitKey(40);
			if (key == 27)
				break;
			else if (key == 32)
				waitKey(0);
		}
		else
		{
			cap.open("1.MOV");
		}
	}
	return 0;
}
