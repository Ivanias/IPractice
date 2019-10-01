#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

Mat img; Mat src_gray;
int thresh = 100; // порог минимума
int max_thresh = 255;
RNG rng(12345);

void thresh_callback(int, void*)
{
	Mat canny_output; // изображения из градаций серого
	vector<vector<Point> > contours; // хранилище памяти для хранения данных найденных контуров
	vector<Vec4i> hierarchy; // указатель, который будет указывать на первый элемент последовательности, содержащей данные найденных контуров

	// позволяет обнаружить края
	Canny(src_gray, canny_output, thresh, thresh * 2, 3);
	/*
	src_gray — одноканальное изображение для обработки (градации серого)
	canny_output — одноканальное изображение для хранения границ, найденных функцией
	thresh — порог минимума (100)
    thresh * 2 — порог максимума (200)
    3 — размер для оператора Собеля (градиент яркости изображения в каждой точке.)
	*/

	// находит контуры
	findContours(canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
	/*
	 canny_output — изображения из градаций серого полученное из Canny()
     contours — хранилище памяти для хранения данных найденных контуров
	 hierarchy — указатель, который будет указывать на первый элемент последовательности, содержащей данные найденных контуров
	 RETR_TREE - найти все контуры и разместить их в иерархии вложенных контуров
	 CHAIN_APPROX_SIMPLE - сжимает горизонтальные, вертикальные и диагональные сегменты и оставляет только их конечные точки
	*/

	// вычисляем моменты
	vector<Moments> mu(contours.size());
	for (int i = 0; i < contours.size(); i++)
	  {
		mu[i] = moments(contours[i], false);
	  }
	
	Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3); // CV_8UC3 - изображение без знака с 3 каналами
	
	// находим центр масс
	vector<Point2f> mc(contours.size());
	for (int i = 0; i < contours.size(); i++)
	  {
		mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00); 
		// центр масс одного объекта - сумма х на их число и сумма у на их число
	  }
	for (int i = 0; i< contours.size(); i++)
	  {
		printf(" Контур № %d: центр масс - x = %.2f  у = %.2f; длина - %.2f \n", i, mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00, arcLength(contours[i], true));
	  }
	
	// рисуем контуры
	for (int i = 0; i < contours.size(); i++)
	  {
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
		circle(drawing, mc[i], 4, color, -1, 5, 0);
		/*
		drawing — изображение на котором будут нарисованы контуры
		mc[i] — указатель на первый контур
		4 — цвет внешних контуров
		color — цвет внутренних контуров(отверстие)
		-1 - будут нарисованы контуры на предыдущем уровне перед contour.
		8 — толщина линии для отображения контуров 
		line_type — тип линии
		*/
	}

	// выводим в окно
	namedWindow("Контуры", WINDOW_AUTOSIZE);
	imshow("Контуры", drawing);
}

int main()
{
	// устанвливаем русскую локаль для консоли
	setlocale(LC_ALL, "Russian");
	
	char filename[80]; // road.jpg

	cout << "Введите имя файла, в который хотите внести изменения, и нажмите Enter" << endl;
	cin.getline(filename, 80);
	cout << "Открыт файл ";
	cout << filename << endl;
    
	// загружаем изображение
    Mat img = imread(filename, 1 );
    
    // конвертируем изображение в серый и затуманиваем его
    cvtColor(img, src_gray, COLOR_RGB2GRAY);
    blur(src_gray, src_gray, Size(3,3) );
    
    /// создаем окно
    char* source_window = "Исходное изображение";
    namedWindow(source_window, WINDOW_AUTOSIZE);
    imshow(source_window, img ); // метод для отображения, 
	//source_window - это название окна, в котором будет отрисовано изображение,
	//src –  имя переменной, которая хранит данное изображение. 
    
    createTrackbar( " Canny thresh:", "Исходное изображение", &thresh, max_thresh, thresh_callback );
    thresh_callback( 0, 0 );
	/*" Canny thresh:" — название ползунка
		"Source" — название окна, в котором будет отображаться ползунок
		&thresh — указатель на целочисленную переменную, которая хранит текущую позицию ползунка.
		max_thresh — максимальная позиция ползунка(минимальная всегда 0).
		thresh_callback — указатель на функцию, которая будет вызываться каждый раз при изменении положения ползунка.
		*/
    waitKey(0); //  Данная команда останавливает выполнение скрипта до нажатия клавиши на клавиатуре. 
	// Параметр 0 означает что нажатие любой клавиши будет засчитано.
    return(0);
}