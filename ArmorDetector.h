#pragma once
#include "opencv2/opencv.hpp"
#include "iostream"
using namespace cv;
using namespace std;

struct ArmorAdjunct
{
	uchar LightThreshold;
	ArmorAdjunct()
	{
		LightThreshold = 220;
	}
};

class ArmorDetector
{
public:
	ArmorDetector(const ArmorAdjunct & adjunct = ArmorAdjunct())
	{
		_adjunct = adjunct;
	};
	~ArmorDetector();
bool imageSet(Mat &srcImage);
private:
	ArmorAdjunct _adjunct;
	Mat lightImage;
	vector<Mat> lightImageCH;
	Mat lightImage_binary;
	Rect ROIrect;
	Mat lightROI;
};

