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
	for (int i = 0; i < 4; i++)
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
	Mat roiImage(r1.size(), CV_8UC3);
	roiImage = srcImage(r1);
	//	pyrUp(roiImage, roiImage, Size(roiImage.cols * 2, roiImage.rows * 2));

	cv::split(roiImage, lightImageCH);
	cv::threshold(lightImageCH.at(0), lightImage_binary, _adjunct.LightThreshold, 255, THRESH_BINARY);
	cv::threshold(lightImageCH.at(1), lightImage_G, _adjunct.LightThreshold, 255, THRESH_BINARY);
	//	Mat element__ = cv::getStructuringElement(1, cv::Size(3, 3));
	//	cv::morphologyEx(lightImage_binary, lightImage_binary, cv::MORPH_CLOSE, element);

	//	cout << r1.size() << endl;
	//	imshow("YCrCb", YCrCbImage_b);


	ROIrect = r1;
	vector<vector<Point>> contours2;
	findContours(lightImage_binary, contours2, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	vector<vector<Point2i> >::const_iterator it = contours2.begin();
	Rectall.clear();
	RectScelet.clear();
	vector<float> l_Vote, r_Vote;
	while (it != contours2.end())
	{
		Rect rect = cv::boundingRect(*it);
		if (((rect.height < r1.height / 14 - 1)) ||
			(rect.height > 14 && rect.width > 0.7 * rect.height + 0.5) ||
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
			//			cout << RectNum << "个 ：" << Rectall.at(RectNum).height << endl;
			RectNum++;
			++it;
		}
	}


	//	int l_vote = 0, r_vote = 0;
	//	int l_vote_ = 0, r_vote_ = 0;
	int RectLnum = 0, RectRnum = 0;
	//	vector<>
	for (int j = 0; j < RectNum - 1; j++)
	{
		for (int i = 0; i < RectNum - 1 - j; i++)
		{
			if (Rectall.at(i).tl().x > Rectall.at(i + 1).tl().x)
			{
				Rect temp;
				temp = Rectall.at(i);
				Rectall.at(i) = Rectall.at(i + 1);
				Rectall.at(i + 1) = temp;
			}
		}
	}
	for (int j = 0; j < RectNum - 1; j++)
	{
		int min = 65535;
		int xxoo = 0;
		for (int i = j; i < RectNum; i++)
		{
			float k1 = getSlope(Rectall.at(j).tl(), Rectall.at(i).tl());
			float k2 = getSlope(Rectall.at(j).br(), Rectall.at(i).br());

			if (!((k1<0.5 && k1>-0.5) && (k2<0.5 && k2>-0.5)))
				continue;
			if ((k1 - k2) > 0.732 || (k2 - k1) > 0.732)
				continue;
			//			cout << "第" << j << "个和第" << i << "个" << "k1:" << k1 << "  k2:" << k2 << endl;
			if (Rectall.at(j).tl().x < Rectall.at(i).tl().x)
			{
				int now = (Rectall.at(i).tl().x - Rectall.at(j).tl().x);
				//				cout << "第" << j << "个和第" << i << "个" << "距离:" << now << endl;
				if (now <= min)
				{
					//					cout << "第" << j << "个和第" << i << "个" << "距离:" << now << endl;
					min = now;
					xxoo = i;
				}
			}
		}
		Rect a = getRectAandB(Rectall.at(j), Rectall.at(xxoo));

		if (a.height < 2 * Rectall.at(j).height)
			if (a.width > a.height / 4 * 3)
				RectScelet.insert(RectScelet.end(), a);
	}

	/*rectangle(roiImage, Rect(Rectall.at(RectLnum).tl(), Rectall.at(RectRnum).br()),
	Scalar(rng.uniform(0, 0), rng.uniform(80, 125), rng.uniform(80, 255)), -1);*/
	//cout << "L:" << RectLnum << "   R:" << RectRnum << endl;
	//cout << "Lrect:" << l_Vote.at(RectLnum) << "   Rrect:" << r_Vote.at(RectRnum) << endl;
	//rectangle(srcImage, r1.tl(), r1.br(),
	//	Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)), 2);
	//cout << RectScelet.size() << endl;
	int block_min = 65535, minnum = 0;
	Rect finalRect;
	vector<long long> block_all;
	for (int i = 0; i < RectScelet.size(); i++)
	{
		Mat Image = lightImageCH.at(1)(RectScelet.at(i));
		long long block = 0;
		long long block_Central = 0;
		int count_c = 0, count_t = 0;
		long long block_Twosides = 0;
		for (int y = 0; y < Image.rows; y++)
			for (int x = 0; x < Image.cols; x++)
			{
				block += Image.at<uchar>(y, x);
				if ((x >= (Image.cols / 2) - 2) && (x <= (Image.cols / 2) + 2))
				{
					//block_Central += Image.at<char>(y, x);
					//count_c++;
					if (Image.at<uchar>(y, x) > 150)
					{
						block += 65535;
					}
				}
				//if (x == 0 || x == (Image.cols - 1))
				//{
				//	block_Twosides += Image.at<char>(y, x);
				//	count_t++;
				//}
			}
		//block_Central /= count_c;
		//block_Twosides /= count_t;
		block /= Image.rows*Image.cols + 1;
    	cout << "第" << i << " " << "block:" << block << "高度：" << Image.rows << endl;
		//cout << Image << endl;
		if (block <= block_min)
		{
			block_min = block;
			minnum = i;
			finalRect = RectScelet.at(i);
			//			cout << "第" << i << " " << "block:" << block << "高度：" << Image.rows << endl;
		}
		block_all.insert(block_all.end(), block);
		//switch (i % 3)
		//{
		//case 0:rectangle(roiImage, RectScelet.at(i), Scalar(0, 255, 0), 1); break;
		//case 1:rectangle(roiImage, RectScelet.at(i), Scalar(255, 0, 0), 1); break;
		//case 2:rectangle(roiImage, RectScelet.at(i), Scalar(0, 0, 255), 1); break;
		//}
	}
	
	for (int i = 0; i < block_all.size(); i++)
	{
		if (i == minnum)
			continue;
		if (block_all.at(i) - block_min < 2)
			if (RectScelet.at(i).width > finalRect.width)
			{
				finalRect = RectScelet.at(i);
				cout << "sss" << i << endl;
			}
	}
	cout << "      " << endl;
	Point center = getCenterPoint(finalRect);
	//	line(srcImage, Point(center.x + r1.x, center.y + r1.y - 20), Point(center.x + r1.x, center.y + r1.y + 20), Scalar(0, 0, 255),1);
	//	line(srcImage, Point(center.x + r1.x - 20, center.y + r1.y), Point(center.x + r1.x + 20, center.y + r1.y), Scalar(0, 0, 255), 1);
	circle(roiImage, center, 2, Scalar(0, 0, 255), -1);
	//	circle(roiImage, center, 10, Scalar(0, 0, 255), 1);
	//	finalRect &= Rect(Point(0, 0), Point(1280, 720));
	//if(!finalRect.empty())
	//	imshow("ssss", lightImageCH.at(1)(finalRect));
	//	rectangle(roiImage, finalRect, Scalar(0, 255, 0), 1);
	imshow("roi", roiImage);
	imshow("阈值化B", lightImage_binary);
	imshow("阈值化G", lightImage_G);
	roiImage.release();
	return true;
}
inline Rect getRectAandB(Rect &A, Rect &B)
{
	if (A.y < B.y)
		return Rect(Point(A.x + A.width, A.y), Point(B.x, B.y + B.height));
	else
		return Rect(Point(A.x + A.width, A.y + A.height), Point(B.x, B.y));
}
inline float getSlope(Point& a, Point& b)
{
	if (a.x == b.x)
		return 10.0f;
	else
		return (float)(a.y - b.y) / (a.x - b.x);
}
inline Point getCenterPoint(Rect &rect)
{
	Point cpt;
	cpt.x = rect.x + cvRound(rect.width / 2.0);
	cpt.y = rect.y + cvRound(rect.height / 2.0);
	return cpt;
}
ArmorDetector::~ArmorDetector()
{
}
