#pragma once
#include "opencv2/opencv.hpp"
using namespace cv;

struct ArmorAdjunct
{
	uchar LightThreshold;
	ArmorAdjunct()
	{
		LightThreshold = 210;
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
	ImageSet(Mat &srcImage);
private:
	ArmorAdjunct _adjunct;
	Mat lightImage;
	Mat lightImage_binary;
};

