#include "ArmorDetector.h"

bool ArmorDetector::imageSet(Mat &srcImage)
{
	RectNum = 0;
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
	r1 &= Rect(Point(0, 0), Point(1280, 720));
	Mat roiImage(r1.size(),CV_8UC3);
	roiImage = srcImage(r1);
//	pyrUp(roiImage, roiImage, Size(roiImage.cols * 2, roiImage.rows * 2));

	cv::split(roiImage, lightImageCH);
	cv::threshold(lightImageCH.at(0), lightImage_binary, _adjunct.LightThreshold, 255, THRESH_BINARY);
	cv::threshold(lightImageCH.at(1), lightImage_G, _adjunct.LightThreshold, 255, THRESH_BINARY);
//	Mat element__ = cv::getStructuringElement(1, cv::Size(3, 3));
//	cv::morphologyEx(lightImage_binary, lightImage_binary, cv::MORPH_CLOSE, element);
	
	cout << r1.size() << endl;
//	imshow("YCrCb", YCrCbImage_b);
	
	
	ROIrect = r1;
	vector<vector<Point>> contours2;
	findContours(lightImage_G, contours2, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	vector<vector<Point2i> >::const_iterator it = contours2.begin();
	Rectall.clear();

	vector<float> l_Vote, r_Vote;
	while (it != contours2.end()) 
	{
		Rect rect = cv::boundingRect(*it);
		if ((rect.height < r1.height/14 - 1) ||
			(rect.height > 14 && rect.width > 0.6 * rect.height + 0.5) ||
			(rect.height <= 14 && rect.width > 1.5 * rect.height))
		{
			it = contours2.erase(it);
		}
		else
		{
			//++i;
			Rectall.insert(Rectall.end(), rect);
			l_Vote.insert(l_Vote.end(), 0);
			r_Vote.insert(r_Vote.end(), 0);
			rectangle(roiImage, Rectall.at(RectNum), Scalar(rng.uniform(0, 0), rng.uniform(80, 125), rng.uniform(80, 255)), 2);
			cout << RectNum << "个 ：" << Rectall.at(RectNum).height << endl;
			RectNum++;
			++it;
		}
	}


//	int l_vote = 0, r_vote = 0;
//	int l_vote_ = 0, r_vote_ = 0;
	int RectLnum = 0, RectRnum = 0;
	for (int j = 0; j < RectNum - 1; j++)
	{
		for(int i = j + 1; i < RectNum; i++)
		{
			float k1 = getSlope(Rectall.at(j).tl(), Rectall.at(i).tl());
			float k2 = getSlope(Rectall.at(j).br(), Rectall.at(i).br());
			cout << "第" << j << "个和第" << i << "个" << "k1:" << k1 << "  k2:" << k2 << endl;
			if (!((k1<1 && k1>-1) && (k2<1 && k2>-1)))
				continue;
			if ((k1-k2) > 0.732 || (k2-k1) > 0.732)
				continue;
			
			if (Rectall.at(j).tl().x > Rectall.at(i).tl().x)
			{
				if (k1 > 0)
				{
					r_Vote.at(j) += (1 - k1);
					l_Vote.at(i) += (1 - k1);
				}
				else
				{
					r_Vote.at(j) += (1 + k1);
					l_Vote.at(i) += (1 + k1);
				}
			}
			else
			{
				if (k1 > 0)
				{
					l_Vote.at(j) += (1 - k1);
					r_Vote.at(i) += (1 - k1);
				}
				else
				{
					l_Vote.at(j) += (1 + k1);
					r_Vote.at(i) += (1 + k1);
				}
			}
		}
	}
	float l_Vote_Max = 0;
	float r_Vote_Max = 0;
	for (int i = 0; i < RectNum; i++)
	{
		if (l_Vote.at(i) >= l_Vote_Max)
		{
			RectLnum = i;
			l_Vote_Max = l_Vote.at(i);
		}
		if (r_Vote.at(i) >= r_Vote_Max)
		{
			RectRnum = i;
			r_Vote_Max = r_Vote.at(i);
		}
	}
	rectangle(roiImage, Rect(Rectall.at(RectLnum).tl(), Rectall.at(RectRnum).br()), 
		Scalar(rng.uniform(0, 0), rng.uniform(80, 125), rng.uniform(80, 255)), -1);
	cout << "L:" << RectLnum << "   R:" << RectRnum << endl;
	cout << "Lrect:" << l_Vote.at(RectLnum) << "   Rrect:" << r_Vote.at(RectRnum) << endl;
	//rectangle(srcImage, r1.tl(), r1.br(),
	//	Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)), 2);
	imshow("roi", roiImage);
//	imshow("阈值化B", lightImage_binary);
	imshow("阈值化G", lightImage_G);
	roiImage.release();
	return true;
}

inline float getSlope(Point& a, Point& b)
{
	if (a.x == b.x)
		return 10.0f;
	else
		return (float)(a.y - b.y) / (a.x - b.x);
}

ArmorDetector::~ArmorDetector()
{
}
