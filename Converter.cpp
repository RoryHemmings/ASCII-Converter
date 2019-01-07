/*

*/

#include <Windows.h>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>
#include <iomanip>
#include <vector>

#include "gstc.h"

using namespace cv;
using namespace std;

char gstc::convert(int intensity) {
	return gstc::chars[(int) (intensity / gstc::conv)];
}

char gstc::convert_s(int intensity) {
	return gstc::chars_s[len_s - (int)(intensity / gstc::conv_s) - 1];
}

BOOL SetConsoleSize(HANDLE & hConsole, SHORT nScreenWidth, SHORT nScreenHeight, SMALL_RECT & srBufferRect) {
	COORD largest, bufferSize;

	srBufferRect = { 0, 0, 1, 1 };
	if (!SetConsoleWindowInfo(hConsole, TRUE, &srBufferRect)) goto Error;

	largest = GetLargestConsoleWindowSize(hConsole);
	bufferSize = { min(largest.X, nScreenWidth), min(largest.Y, nScreenHeight) };
	if (!SetConsoleScreenBufferSize(hConsole, bufferSize)) goto Error;

	srBufferRect = { 0, 0, bufferSize.X - 1, bufferSize.Y - 1 };
	if (!SetConsoleWindowInfo(hConsole, TRUE, &srBufferRect)) goto Error;

	return TRUE;
Error:
	CloseHandle(hConsole);
	return FALSE;
}

BOOL SetConsoleFont(HANDLE & hConsole, SHORT nFontSize, const CHAR * szFontName) {
	CONSOLE_FONT_INFOEX cfie;
	ZeroMemory(&cfie, sizeof(cfie));
	cfie.cbSize = sizeof(cfie);
	cfie.dwFontSize.Y = nFontSize;
	lstrcpyW(cfie.FaceName, (LPCWSTR)szFontName);

	if (SetCurrentConsoleFontEx(hConsole, TRUE, &cfie)) return TRUE;

	CloseHandle(hConsole);
	return FALSE;
}

template <class A>
void print2dArray(A input, int height, int width) {
	for (int row = 0; row < height; ++row) {
		for (int col = 0; col < width; ++col) {
			cout << setw(3) << (int) input[row][col] << " ";
		}
		cout << endl;
	}
}

void setPixel(Mat& img, int row, int col, int val) {
	img.at<uchar>(row, col) = val;
}

Mat downscale(const Mat& img, int scale) {
	int rows = img.rows / scale;
	int cols = img.cols / scale;

	Mat ret(rows, cols, CV_8UC1, Scalar(0));
	for (int row = 0; row < rows; ++row) {
		for (int col = 0; col < cols; ++col) {
			ret.at<uchar>(row, col) = img.at<uchar>(row * scale, col * scale);
		}
	}

	return ret;
}

Mat upscale(const Mat& img, int scale) {
	int rows = img.rows * scale;
	int cols = img.cols * scale;

	Mat ret(rows, cols, CV_8UC1, Scalar(0));
	for (int row = 0; row < rows; ++row) {
		for (int col = 0; col < cols; ++col) {
			ret.at<uchar>(row, col) = img.at<uchar>(row / scale, col / scale);
		}
	}

	return ret;
}

Mat stretch(const Mat& img, int scale) {
	const int cols = img.cols * scale;
	Mat ret(img.rows, cols, CV_8UC1, Scalar(0));
	for (int row = 0; row < img.rows; ++row) {
		for (int col = 0; col < ret.cols; ++col) {
			ret.at<uchar>(row, col) = img.at<uchar>(row, col / scale);
		}
	}

	return ret;
}

int main(int argc, char** argv)
{
	// Read the image file
	Mat reference = imread("C:/Users/RPMHe/Desktop/linux.png", 0);          // REFERENCE IMAGE DECLARATION

	if (reference.empty()) // Check for failure
	{
		cout << "Could not open or find the image" << endl;
		system("pause"); //wait for any key press
		return -1;
	}

	Mat img = downscale(stretch(reference, 2), 16);							// WORKING IMAGE

	const int nImageHeight = img.rows;
	const int nImageWidth = img.cols;

	// Mat img(nImageHeight, nImageWidth, CV_8UC1, Scalar(230));
	uchar** vals = new uchar*[nImageHeight];
	for (int i = 0; i < nImageHeight; ++i) {
		vals[i] = new uchar[nImageWidth];
	}

	for (int row = 0; row < nImageHeight; ++row) {
		for (int col = 0; col < nImageWidth; ++col) {
			uchar intensity = img.at<uchar>(row, col);
			vals[row][col] = intensity;
		}
	}

	//print2dArray(vals, nImageHeight, nImageWidth);

	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	vector<CHAR_INFO> buffer = std::vector<CHAR_INFO>(nImageWidth * nImageHeight);

	SMALL_RECT consoleWriteArea = { 0, 0, (short) nImageWidth - 1, (short) nImageHeight - 1 };
	SetConsoleScreenBufferSize(hConsole, { (short) nImageWidth,(short) nImageHeight });
	if (!SetConsoleWindowInfo(hConsole, TRUE, &consoleWriteArea)) std::cout << GetLastError() << std::endl;
	if (!SetConsoleScreenBufferSize(hConsole, { (short) nImageWidth, (short) nImageHeight })) std::cout << GetLastError() << std::endl;
	SetConsoleActiveScreenBuffer(hConsole);

	CONSOLE_SCREEN_BUFFER_INFOEX csbi;
	csbi.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
	GetConsoleScreenBufferInfoEx(hConsole, &csbi);

	for (int i = 0; i < 16; ++i) {
		csbi.ColorTable[i] = RGB(i * (255 / 16), i * (255 / 16), i * (255 / 16));
	}
	csbi.ColorTable[0] = 0;

	SetConsoleScreenBufferInfoEx(hConsole, &csbi);

	SetConsoleFont(hConsole, 8, "Anonymous Pro");								// FONT SIZE
	SetConsoleSize(hConsole, nImageWidth, nImageHeight, consoleWriteArea);

	String windowName = "Window";
	namedWindow(windowName);
	imshow(windowName, downscale(reference, 2));								// REFERENCE IMAGE DECLARATION

	while (true) {
		for (int i = 0; i < nImageWidth * nImageHeight; ++i) {
			buffer[i].Char.AsciiChar = ' ';
			buffer[i].Attributes = 0;
		}

		for (int y = 0; y < nImageHeight; ++y) {
			for (int x = 0; x < nImageWidth; ++x) {
				double s = 16.0 / 255;
				buffer[x + y * nImageWidth].Char.AsciiChar = gstc::convert(vals[y][x]);
				//buffer[x + y * nImageWidth].Char.AsciiChar = (char) 219;
				buffer[x + y * nImageWidth].Attributes = (s * vals[y][x]);
				//buffer[x + y * nImageWidth].Attributes = 15;
			}
		}

		if (!WriteConsoleOutputA(hConsole, &buffer[0], { (short)nImageWidth, (short)nImageHeight }, { 0, 0 }, &consoleWriteArea)) std::cout << GetLastError() << std::endl;
		waitKey(1000);
	}

	destroyWindow(windowName);

	return 0;
}

