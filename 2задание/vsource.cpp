#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
# define M_PIl 3.141592653589793238462643383279502884L

using namespace cv;
using namespace std;

int main() 
{
	// ������������ ������� ������ ��� �������
	setlocale(LC_ALL, "Russian");

	char filename[100]; // Figyres.mp4 ��� Figyres_same_colour.mp4
	cout << "������� ��� �����, ������� ������ ����������������, � ������� Enter" << endl;
	cin.getline(filename, 100);
	cout << "������ ���� ";
	cout << filename << endl;

	VideoCapture cap(filename);
	if (!cap.isOpened()) { cout << "������ �������� ����� "; return -1; }
	Mat im;
	for (; ;) 
	  {
		Mat mat, frame;
		cap >> frame;
		mat = frame;
		if (mat.empty()) break;

		cvtColor(mat, mat, COLOR_RGB2GRAY);
		GaussianBlur(mat, mat, Size(3, 3), 0);
		Canny(mat, mat, 50, 100);

		vector<vector<Point>> points;
		auto kn = getStructuringElement(MORPH_RECT, Size(3, 3));
		dilate(mat, mat, kn);
		findContours(mat, points, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);


		for (int i = 0; i < points.size(); i++) 
		  {
			vector<Point> _points;
			double len = arcLength(Mat(points[i]), true);
			if (len < 30) continue;
			approxPolyDP(points[i], _points, len*0.02, true);
			Moments m = moments(_points);
			Point centm(m.m10 / m.m00, m.m01 / m.m00);
			printf(" ������ � %d: ����� ���� - � = %.2f, � = %.2f \n", i, m.m10 / m.m00, m.m01 / m.m00);

			// 4 ������� - �������������
			if (_points.size() == 4) 
			 {
				drawContours(frame, vector<vector<Point>>{_points}, 0, Scalar(255, 255, 0), 2);
				drawMarker(frame, centm, Scalar(255, 255, 255));
				continue;
			 }
			// 3 ������� - �����������
			if (_points.size() == 3) 
			 {
				drawContours(frame, vector<vector<Point>>{_points}, 0, Scalar(0, 255, 255), 2);
				drawMarker(frame, centm, Scalar(255, 255, 255));
				continue;
			 }
			// ���� �� ������������� � �� �����������
			if (_points.size() > 4 || _points.size() < 3) 
			 {
				Point2f cent;
				float rad;
				minEnclosingCircle(points[i], cent, rad);
				if ((2 * rad*M_PIl - len)*(2 * rad*M_PIl - len) < len*0.2) 
				 {
					circle(frame, cent, rad, Scalar(0, 255, 0), 2);
				 }
				continue;
			 }
		  }
		imshow("�������������", frame);
		if (waitKey(30) >= 0) break;
	}
	return 0;
}