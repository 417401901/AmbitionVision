#include "ArmorDetector.h"

bool ArmorDetector::imageSet(Mat &srcImage)
{
	Mat YCrCbImage, YCrCbImage_b;
	vector<Mat> channels;
	srcImage.copyTo(lightImage);
	cv::cvtColor(lightImage, YCrCbImage, COLOR_BGR2YCrCb);
	cv::split(YCrCbImage, channels);	
	cv::threshold(channels.at(2), YCrCbImage_b, 170, 255, THRESH_BINARY);
	Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
	cv::morphologyEx(YCrCbImage_b, YCrCbImage_b, cv::MORPH_CLOSE, element);
	Mat element_ = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(9, 9));
	for(int i = 0; i < 4; i ++)
		cv::dilate(YCrCbImage_b, YCrCbImage_b, element_);
	vector<vector<Point>> contours;
	findContours(YCrCbImage_b, contours, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	vector<Rect> boundRect(contours.size());
	cv::RNG rng(time(0));
	int num = 0, Area = 0;
	for (int i = 0; i < contours.size(); i++)
	{
		boundRect[i] = boundingRect(Mat(contours[i]));
//		drawContours(srcImage, contours, i, Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)), 2);
		if (boundRect[i].area() > Area)
		{
			num = i;
			Area = boundRect[i].area();
		}
	}
	Point p1(boundRect[num].tl().x - boundRect[num].width / 8, boundRect[num].tl().y);
	Point p2(boundRect[num].br().x + boundRect[num].width / 8, boundRect[num].br().y);
	Rect r1(p1, p2);
	for (int i = 0; i < contours.size(); i++)
	{
		if (!(r1&boundRect[i]).empty() && i != num)
		{
			r1 |= boundRect[i];
			Point p1(r1.tl().x - r1.width / 6, r1.tl().y);
			Point p2(r1.br().x + r1.width / 6, r1.br().y);
			r1 = Rect(p1, p2);
		}
	}
	
	Mat roiImage(r1.size(),CV_8UC3);
	roiImage = srcImage(r1);
//	pyrUp(roiImage, roiImage, Size(roiImage.cols * 2, roiImage.rows * 2));

	cv::split(roiImage, lightImageCH);
	cv::threshold(lightImageCH.at(0), lightImage_binary, _adjunct.LightThreshold, 255, THRESH_BINARY);
	
	
//	cout << roiImage.size() << r1.size() << endl;
//	imshow("YCrCb", YCrCbImage_b);
	
	
	ROIrect = r1;
	vector<vector<Point>> contours2;
	findContours(lightImage_binary, contours2, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	vector<vector<Point2i> >::const_iterator it = contours2.begin();
	while (it != contours2.end()) 
	{
		Rect rect = cv::boundingRect(*it);
		if ((rect.height < 3) ||
			(rect.height > 7 && rect.width > 0.6 * rect.height + 0.5) ||
			(rect.height <= 7 && rect.width > 0.9 * rect.height))
		{
			++it;
			continue;
		}
		rectangle(roiImage, rect, Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)), 2);
		++it;
	}
	rectangle(srcImage, r1.tl(), r1.br(),
		Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)), 2);
	imshow("roi", roiImage);
	imshow("„–÷µªØ", lightImage_binary);
	roiImage.release();
	return true;
}



ArmorDetector::~ArmorDetector()
{
}
