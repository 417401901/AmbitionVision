#pragma once
#include "opencv2/opencv.hpp"
#include "iostream"
using namespace cv;
using namespace std;

inline float getSlope(Point& a, Point& b);
inline Point getCenterPoint(Rect &rect);
inline Rect getRectAandB(Rect &A, Rect &B);
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
	Mat lightImage_G;
	int RectNum;
	vector<Rect> Rectall;
	vector<Rect> RectScelet;
	Rect ROIrect;
	Mat lightROI;
};

