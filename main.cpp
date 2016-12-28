#include <stdio.h>
#include"image_bmp.h"
/*
#include<opencv2/opencv.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/core.hpp>

#ifdef _DEBUG
#pragma comment(lib,"opencv_world310d.lib")
#else
#pragma comment(lib,"opencv_world310.lib")
#endif
*/
int main() {
	IMAGE_TYPE img;
	if (ReadBMP("Digimon24.bmp", &img, NULL)==IMAGE_FAILURE){
		return EXIT_FAILURE;
	}
	WriteBMP("result.bmp", &img, 24, NULL);

	return 0;
	/*
	cv::Mat mat = cv::Mat::zeros(img.height, img.width, CV_8UC3);
	for (int y = 0; y < img.height; y++){
		for (int x = 0; x < img.width; x++){
			mat.at<cv::Vec3b>(y, x)[0] = img.data[y][x].b;
			mat.at<cv::Vec3b>(y, x)[1] = img.data[y][x].g;
			mat.at<cv::Vec3b>(y, x)[2] = img.data[y][x].r;
		}
	}
	cv::imshow("img", mat);
	cv::waitKey();
	cv::destroyAllWindows();
	*/
	

	//24비트 BMP 읽는거랑 저장 마저 짜면되.
}