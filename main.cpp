#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>

using namespace std;

/*

TRACK A YELLOW BALL - OBJECT DETECTION METHOD USING COLOR SEPERATION OPEN CV 3.1.0

author - Rachit Gulati

*/

int main() 
{
	
	cv::VideoCapture capcam("teste.mp4");									//abre um vídeo ou webcam

	if (capcam.isOpened() == false)											//checa se foi aberto com sucesso
	{
		std::cout << "error: Webcam connect unsuccessful\n";				// if not then print error message
		return(0);
	}

	cv::Mat imgOriginal;	// Input image
	cv::Mat hsvImg;			// HSV Image
	cv::Mat threshImg;		// Thresh Image

	vector<cv::Vec3f>  v3fCircles;  // 3 element vector of floats, this will be the pass by reference output of HoughCircles()

	char charCheckForEscKey = 0;

	int lowH = 0;       // Set Hue
	int highH = 179;
	int lowS = 0;       // Set Saturation
	int highS = 255;
	int lowV = 0;       // Set Value
	int highV = 136;


	double elapsed_time;
	vector<double> vTempos;
	vector<float> vPos;

	while (charCheckForEscKey != 27 && capcam.isOpened())
	{

		bool blnFrameReadSuccessfully = capcam.read(imgOriginal);  // get next frame

		if (!blnFrameReadSuccessfully || imgOriginal.empty())
		{    // if frame read unsuccessfully
			std::cout << "error: frame can't read \n";      // print error message
			break;               // jump out of loop

		}

		cv::cvtColor(imgOriginal, hsvImg, cv::COLOR_BGR2HSV);										// Convert Original Image to HSV Thresh Image
		cv::inRange(hsvImg, cv::Scalar(lowH, lowS, lowV), cv::Scalar(highH, highS, highV), threshImg);
		cv::GaussianBlur(threshImg, threshImg, cv::Size(3, 3), 0);								//Blur Effect
		cv::dilate(threshImg, threshImg, 0);													// Dilate Filter Effect
		cv::erode(threshImg, threshImg, 0);														// Erode Filter Effect


		// fill circles vector with all circles in processed image

		cv::HoughCircles(threshImg, v3fCircles, cv::HOUGH_GRADIENT, 2, threshImg.rows / 4, 100, 50, 10, 800);  // algorithm for detecting circles  


		for (int i = 0; i < v3fCircles.size(); i++)
		{      // for each circle
			std::cout << "Ball position X = " << v3fCircles[i][0]						// x position of center point of circle
				<< ",\tY = " << v3fCircles[i][1]										// y position of center point of circle
				<< ",\tRadius = " << v3fCircles[i][2] << "\n";							// radius of circle
								
			cv::circle(imgOriginal, cv::Point((int)v3fCircles[i][0], (int)v3fCircles[i][1]), 3, cv::Scalar(0, 255, 0), cv::FILLED);        // draw small green circle at center of object detected     							 
			cv::circle(imgOriginal, cv::Point((int)v3fCircles[i][0], (int)v3fCircles[i][1]), (int)v3fCircles[i][2], cv::Scalar(0, 0, 255), 3);         // draw red circle around object detected      

			if (v3fCircles[i][0])									//se for 0 não ira contar como valor analisado
			{
				vPos.push_back(v3fCircles[i][0]);
				elapsed_time = (double)capcam.get(cv::CAP_PROP_POS_FRAMES) / (double)30;
				vTempos.push_back(elapsed_time);											//salva os tempos de frame para fazer o grafico
			}
			{
				// declare windows

				cv::namedWindow("imgOriginal", cv::WINDOW_AUTOSIZE);
				cv::namedWindow("threshImg", cv::WINDOW_AUTOSIZE);


				/* Create trackbars in "threshImg" window to adjust according to object and environment.*/

				cv::createTrackbar("LowH", "threshImg", &lowH, 179); //Hue (0 - 179)
				cv::createTrackbar("HighH", "threshImg", &highH, 179);
				cv::createTrackbar("LowS", "threshImg", &lowS, 255); //Saturation (0 - 255)
				cv::createTrackbar("HighS", "threshImg", &highS, 255);
				cv::createTrackbar("LowV", "threshImg", &lowV, 255); //Value (0 - 255)
				cv::createTrackbar("HighV", "threshImg", &highV, 255);
				cv::imshow("imgOriginal", imgOriginal);     // show windows
				cv::imshow("threshImg", threshImg);
			}


			charCheckForEscKey = cv::waitKey(1);     // delay and get key press



		}

		ofstream dados;
		dados.open("Dados.txt");
		for (int i = 0; i < vPos.size(); i++)
		{
			dados << vTempos[i] << "    " << vPos[i] << endl;
		}
		dados.close();

	}

	return(0);
}