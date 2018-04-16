#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <iostream>
#include <thread>
#include <iomanip>
using namespace std;
using namespace cv;

//per convertire in stringa
#define SSTR( x ) static_cast< std::ostringstream & >(( std::ostringstream() << std::dec << x ) ).str()
//GOTURN è buggato almeno in OpenCV 3.2, proviamo su 3.4.0 ora
string trackerTypes[6] = { "BOOSTING", "MIL", "KCF", "TLD","MEDIANFLOW", "GOTURN" };
//creo un tracker, usiamo MIL per iniziare, che è il migliore in genere, anche KCF però non scherza, se la contendono
string trackerType = trackerTypes[2];
Ptr<Tracker> tracker;
Mat src;
char messaggio;
int x1, y11, x2, y2;
int fact = 20;        //fattore di divergenza per l'invio di un'azione come il movimento a destra o sinistra
int px1, py1, px2, py2, fd, k = 0, count;
int main() {
	VideoCapture cap(0);
	if (!cap.isOpened()) {
		cout << "\nImpossibile aprire la webcam!\n";
		return 1;
	}
	//controllo pr la comunicazione seriale
	if ((fd = serialOpen("/dev/ttyUSB0", 9600)) < 0) {
		fprintf(stderr, "Impossibile aprire la comunicazione seriale: %s\n", strerror(errno));
		return 1;
	}
	cout << setfill('-') << setw(35) << "OCTOBOT" << setfill('-') << setw(20) << "\n";
	cap >> src;
	namedWindow("PiCam", CV_WINDOW_AUTOSIZE);
	imshow("PiCam", src);
	//prende il primo frame
	bool ok = cap.read(src);
	//scelta del tracking :
	//si fa scegliere manualmente con il mouse il tracking
	Rect2d bbox = selectROI(src, false);
	//si sceglie la posizione dell'oggetto da tracciare in codice in alternativa
	//Rect2d bbox(287, 23, 86, 320);
	rectangle(src, bbox, Scalar(255, 0, 0), 2, 1);
	imshow("PiCam", src);
	tracker->init(src, bbox);
	while (true) {
		cap >> src;
		double timer = (double)getTickCount();
		bool ok = tracker->update(src, bbox);
		float fps = getTickFrequency() / ((double)getTickCount() - timer);
		if (ok) {
			rectangle(src, bbox, Scalar(255, 0, 0), 2, 1);
			x1 = bbox.x;
			y11 = bbox.y;
			x2 = x1 + bbox.width;
			y2 = y11 + bbox.height;
			if (k > 0) {
				if (x1 > px1 + fact)
					messaggio = 'd';
				if (px1 > x1 + fact)
					messaggio = 's';
				if (((px2 - px1)*(py2 - py1)) > ((x2 - x1)*(y2 - y11)) + fact)
					messaggio = 'a';
				if (((x2 - x1)*(y2 - y11)) > ((px2 - px1)*(py2 - py1)) + fact)
					messaggio = 'i';
			}
			serialPutchar(fd, messaggio);
			px1 = x1;
			py1 = y11;
			px2 = x2;
			py2 = y2;
			k++;
		}
		else
			putText(src, "Errore tracking rilevato", Point(100, 80), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 2);
		//outputta trackerType
		putText(src, trackerType + " Tracker", Point(100, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50, 170, 50), 2);
		//outputta gli fps
		putText(src, "FPS : " + SSTR(int(fps)), Point(100, 50), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50, 170, 50), 2);
		imshow("PiCam", src);
	}	
	return 0;
}
