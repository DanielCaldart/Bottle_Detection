#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

//Function prototypes
void buttonRun(int state, void* userdata);
void buttonEvaluation(int state, void* userdata);
void buttonContours(int state, void* userdata);
void buttonAreaSize(int state, void* userdata);
void buttonBoundingBox(int state, void* userdata);
void buttonAllWindows(int state, void* userdata);
void buttonEmpty(int state, void* userdata);
void buttonTests(int state, void* userdata);

int calculateArea(vector<vector<Point>> contours);
bool evaluation(vector<vector<Point>> contours, int areaBottle, int trackAreaValue);
void drawings(Mat img, vector<vector<Point>> contours, vector<Rect> boundingRectangle, bool evalResult, int areaBottle);
void showWindows(Mat img, Mat imgGray, Mat imgBlur, Mat imgBinary);

bool testButtons(Mat img);
bool testTrackbars(Mat img);
bool testCalculateArea(Mat img);
bool testEvaluation(Mat img);
void drawTestResults(Mat img);

void resetButtons();
void resetTests();

//Initialize global variables
bool buttonRunState = 0;
bool buttonEvaluationState = 0;
bool buttonContoursState = 0;
bool buttonAreaSizeState = 0;
bool buttonBoundingBoxState = 0;
bool buttonAllWindowsState = 0;
bool buttonTestsState = 0;

bool testButtonsResult = 0;
bool testTrackbarsResult = 0;
bool testCalculateAreaResult = 0;
bool testEvaluationResult = 0;

//////////////////////////////////////////////////////////////////////////////////////////////
//		Main
//////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
	//Initialize local variables
	int trackThreshValue = 0;
	const int trackThreshMax = 255;
	const int trackThreshDefault = 200;

	int trackAreaValue = 0;
	const int trackAreaMax = 15000;
	const int trackAreaDefault = 9000;

	int areaBottle = 0;
	bool evalResult = 0;

	Mat img;
	Mat imgGray;
	Mat imgBlur;
	Mat imgBinary;

	vector<vector<Point>> contours;
	vector<vector<Point>> contoursPolygon;
	vector<Rect> boundingRectangle;

	//Initialize video capture
	VideoCapture cap(0);
	cap.set(CAP_PROP_FRAME_WIDTH, 1280);
	cap.set(CAP_PROP_FRAME_HEIGHT, 720);
	int fourcc = VideoWriter::fourcc('M','J','P','G');
	cap.set(CAP_PROP_FOURCC, fourcc);
	cap.set(CAP_PROP_AUTOFOCUS, 0);
	cap.set(CAP_PROP_FOCUS, 150);

	//Initialize windows
	namedWindow("Image");

	//Initialize buttons
	createButton("Run/Stop programm", buttonRun, NULL, QT_PUSH_BUTTON | QT_NEW_BUTTONBAR, 0);
	createButton("Show evaluation", buttonEvaluation, NULL, QT_PUSH_BUTTON | QT_NEW_BUTTONBAR, 0);
	createButton("Show contours", buttonContours, NULL, QT_PUSH_BUTTON | QT_NEW_BUTTONBAR, 0);
	createButton("Show area size", buttonAreaSize, NULL, QT_PUSH_BUTTON | QT_NEW_BUTTONBAR, 0);
	createButton("Show bounding box", buttonBoundingBox, NULL, QT_PUSH_BUTTON | QT_NEW_BUTTONBAR, 0);
	createButton("Show all windows", buttonAllWindows, NULL, QT_PUSH_BUTTON | QT_NEW_BUTTONBAR, 0);
	createButton("Run tests", buttonTests, NULL, QT_PUSH_BUTTON | QT_NEW_BUTTONBAR, 0);

	//Initialize trackbars
	createTrackbar( "Threshold", "Image", nullptr, trackThreshMax);
	setTrackbarPos("Threshold", "Image", trackThreshDefault);
	createTrackbar( "Area Min", "Image", nullptr, trackAreaMax);
	setTrackbarPos("Area Min", "Image", trackAreaDefault);

	//Main loop
	while(waitKey(1) != 27)
	{
		cap.read(img);
		putText( img, "Programm stopped. Run programm or start test sequenz. Press Esc to exit.", Point(20, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);
		imshow("Image", img);

		//Loop for bottle programm
		while(buttonRunState == 1)
		{
			//Capture image and convert into grayscale, add blur and convert into binary
			cap.read(img);
			cvtColor(img, imgGray, COLOR_BGR2GRAY);
			blur(imgGray, imgBlur, Size(10, 10));
			trackThreshValue = getTrackbarPos("Threshold", "Image");
			threshold(imgBlur, imgBinary, trackThreshValue, trackThreshMax, 0);

			//Find Contours
			findContours(imgBinary, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

			//Resize to match the number of contours
			contoursPolygon.resize(contours.size());
			boundingRectangle.resize(contours.size());

			//Turn contours into polygons and calculate bounding boxes
			for(int i = 0; i < contours.size(); i++)
			{
				approxPolyDP( contours[i], contoursPolygon[i], 3, true );
				boundingRectangle[i] = boundingRect( contoursPolygon[i] );
			}

			//Calculate bottle area, decide if bottle is ok and show windows with drawings
			areaBottle = calculateArea(contours);
			trackAreaValue = getTrackbarPos("Area Min", "Image");
			evalResult = evaluation(contours, areaBottle, trackAreaValue);
			drawings(img, contours, boundingRectangle, evalResult, areaBottle);
			showWindows(img, imgGray, imgBlur, imgBinary);
		}

		//Tests
		if(buttonTestsState == 1)
		{
			buttonTestsState = 0;

			//Run tests
			cap.read(img);
			testButtonsResult = testButtons(img);
			cap.read(img);
			testTrackbarsResult = testTrackbars(img);
			cap.read(img);
			testCalculateAreaResult = testCalculateArea(img);
			cap.read(img);
			testEvaluationResult = testEvaluation(img);

			//Draw test results
			cap.read(img);
			drawTestResults(img);

			//Rest buttons, trackbars and tests
			resetButtons();
			resetTests();
			setTrackbarPos("Threshold", "Image", trackThreshDefault);
			setTrackbarPos("Area Min", "Image", trackAreaDefault);
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//	Functions fur buttons
//////////////////////////////////////////////////////////////////////////////////////////////

void buttonRun(int state, void* userdata)
{
	if(buttonRunState == 0)
	{
		buttonRunState = 1;
	}
	else
	{
		buttonRunState = 0;
	}
}

void buttonEvaluation(int state, void* userdata)
{
	if(buttonEvaluationState == 0)
	{
		buttonEvaluationState = 1;
	}
	else
	{
		buttonEvaluationState = 0;
	}
}

void buttonContours(int state, void* userdata)
{
	if(buttonContoursState == 0)
	{
		buttonContoursState = 1;
	}
	else
	{
		buttonContoursState = 0;
	}
}

void buttonAreaSize(int state, void* userdata)
{
	if(buttonAreaSizeState == 0)
	{
		buttonAreaSizeState = 1;
	}
	else
	{
		buttonAreaSizeState = 0;
	}
}

void buttonBoundingBox(int state, void* userdata)
{
	if(buttonBoundingBoxState == 0)
	{
		buttonBoundingBoxState = 1;
	}
	else
	{
		buttonBoundingBoxState = 0;
	}
}

void buttonAllWindows(int state, void* userdata)
{
	if(buttonAllWindowsState == 0)
	{
		buttonAllWindowsState = 1;
	}
	else
	{
		buttonAllWindowsState = 0;
	}
}

void buttonTests(int state, void* userdata)
{
	if(buttonTestsState == 0)
	{
		buttonTestsState = 1;
	}
	else
	{
		buttonTestsState = 0;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
//		Functions for main programm	
//////////////////////////////////////////////////////////////////////////////////////////////

//Calculate area of bottle in pixels
int calculateArea(vector<vector<Point>> contours)
{
	if (contours.size() == 2)
		{
			return contourArea(contours[1]) - contourArea(contours[0]);
		}
		else
		{
			return 0;
		}
}

//Evaluate of bottle is ok or not
bool evaluation(vector<vector<Point>> contours, int areaBottle, int trackAreaValue)
{
	if (areaBottle > trackAreaValue && contours.size() == 2)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//Drwaings form the bottle detection programm onto the image
void drawings(Mat img, vector<vector<Point>> contours, vector<Rect> boundingRectangle, bool evalResult, int areaBottle)
{
	putText( img, "Programm is running.", Point(20, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);

	//Draw contours
	if(buttonContoursState == 1)
	{
		drawContours(img, contours, -1, Scalar(255, 0, 255), 2);
	}

	//Draw outer bounding box, calculate center and draw center
	if (buttonBoundingBoxState == 1 && boundingRectangle.size() >= 1)
	{
		rectangle(img, boundingRectangle[1].tl(), boundingRectangle[1].br(), Scalar(0, 255, 255), 5);
		Point center = Point(boundingRectangle[1].x + boundingRectangle[1].width / 2 , boundingRectangle[1].y + boundingRectangle[1].height / 2);
		circle(img, Point(center),10, Scalar(0, 255, 255),-1, 2 ,0);
	}

	//Print evaluation
	if(buttonEvaluationState == 1 && evalResult == 1)
	{
		putText( img, "Bottle ok", Point(20, 100), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);
	}
	else if(buttonEvaluationState == 1 && evalResult == 0)
	{
		putText( img, "Bottle not ok", Point(20, 100), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);
	}

	//Print area size of bottle
	if(buttonAreaSizeState == 1)
	{
		string areaBottleStr = to_string(areaBottle);
		putText( img, areaBottleStr, Point(20, 150), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);
	}
}

//Show all available images
void showWindows(Mat img, Mat imgGray, Mat imgBlur, Mat imgBinary)
{
	imshow("Image", img);

	if(buttonAllWindowsState == 1)
	{
		namedWindow("Gray");
		namedWindow("Blur");
		namedWindow("Binary");
		imshow("Gray", imgGray);
		imshow("Blur", imgBlur);
		imshow("Binary", imgBinary);
	}
	else
	{
		destroyWindow("Gray");
		destroyWindow("Blur");
		destroyWindow("Binary");
	}
	waitKey(1);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//		Functions for tests	
//////////////////////////////////////////////////////////////////////////////////////////////

//Test if the buttons are working correctly
bool testButtons(Mat img)
{
	putText( img, "Test sequenz is running.", Point(20, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);
	putText( img, "Press all buttons once. Press Esc if you can't finish the test.", Point(20, 100), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);

	bool buttonRunPressed = 0;
	bool buttonEvalPressed = 0;
	bool buttonContoursPressed = 0;
	bool buttonAreaPressed = 0;
	bool buttonBbPressed = 0;
	bool buttonWindowsPressed = 0;
	bool buttonTestsPressed = 0;

	//Print status if any button is pressed
	while(waitKey(1) != 27)
	{
		if(buttonRunState == 1)
		{
			putText( img, "Run/Stop programm ok.", Point(20, 150), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);
			buttonRunPressed = 1;
		}

		if(buttonEvaluationState == 1)
		{
			putText( img, "Show evaluation ok.", Point(20, 200), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);
			buttonEvalPressed = 1;
		}

		if(buttonContoursState == 1)
		{
			putText( img, "Show contours ok.", Point(20, 250), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);
			buttonContoursPressed = 1;
		}

		if(buttonAreaSizeState == 1)
		{
			putText( img, "Show area size ok.", Point(20, 300), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);
			buttonAreaPressed = 1;
		}

		if(buttonBoundingBoxState == 1)
		{
			putText( img, "Show bounding box ok.", Point(20, 350), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);
			buttonBbPressed = 1;
		}

		if(buttonAllWindowsState == 1)
		{
			putText( img, "Show all windows ok.", Point(20, 400), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);
			buttonWindowsPressed = 1;
		}

		if(buttonTestsState == 1)
		{
			putText( img, "Run tests ok.", Point(20, 450), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);
			buttonTestsPressed = 1;
		}

		imshow("Image", img);

		//The test is passed if all buttons were pressed once (expected = 1, returned = 1)
		if(buttonRunPressed == 1 && buttonEvalPressed == 1 && buttonContoursPressed == 1 && buttonAreaPressed == 1 &&
			buttonBbPressed == 1 && buttonWindowsPressed == 1 && buttonTestsPressed == 1)
		{
			return 1;
		}
	}
	return 0;
}

//Test if trackbars are working correctly
bool testTrackbars(Mat img)
{
	putText( img, "Test sequenz is running.", Point(20, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);
	putText( img, "Move each Trackbar to Min and Max. Press Esc if you can't finish the test.", Point(20, 100), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);

	bool thresholdMinResult = 0;
	bool thresholdMaxResult= 0;
	bool areaMinResult = 0;
	bool areaMaxResult = 0;

	int testTrackThreshValue = 0;
	int testTrackAreaValue = 0;

	//Print if trackbars have been moved to each end
	while(waitKey(1) != 27)
	{
		testTrackThreshValue = getTrackbarPos("Threshold", "Image");
		testTrackAreaValue = getTrackbarPos("Area Min", "Image");

		if(testTrackThreshValue == 0)
		{
			putText( img, "Min value threshold trackbar ok", Point(20, 150), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);
			thresholdMinResult = 1;
		}

		if(testTrackThreshValue == 255)
		{
			putText( img, "Max value threshold trackbar ok", Point(20, 200), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);
			thresholdMaxResult = 1;
		}

		if(testTrackAreaValue == 0)
		{
			putText( img, "Min value area trackbar ok", Point(20, 250), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);
			areaMinResult = 1;
		}

		if(testTrackAreaValue == 255)
		{
			putText( img, "Max value area trackbar ok", Point(20, 300), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);
			areaMaxResult = 1;
		}

		imshow("Image", img);

		//The test is passed when the actual trackbar values matched the end values
		if(thresholdMinResult == 1 && thresholdMaxResult == 1 &&
			areaMinResult == 1 && areaMaxResult == 1)
		{
			return 1;
		}
	}
	return 0;
}

//Test if function to calculate bottle area in pixel works correctly
bool testCalculateArea(Mat img)
{
	putText( img, "Test sequenz is running.", Point(20, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);
	putText( img, "Testing calculation of area function.", Point(20, 100), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);

	bool areaEmptyResult = 1;
	bool areaFullResult = 0;

	vector<vector<Point>> testEmpty;
	vector<vector<Point>> testFull{{Point(1, 1), Point(1, 2), Point(2, 2), Point(2, 1)}, {Point(1, 1), Point(1, 3), Point(2, 3), Point(2, 1)}};

	//Test function with empty vector (expected result = 0)
	areaEmptyResult = calculateArea(testEmpty);

	//Test function with test vector (expected result = 1)
	areaFullResult = calculateArea(testFull);

	imshow("Image", img);
	waitKey(2000);

	//Test is passed when actual results match expected results
	if(areaEmptyResult == 0 && areaFullResult == 1)
	{
		return 1;
	}
	return 0;
}

//Test if function to evaluate if bottle is ok works correctly
bool testEvaluation(Mat img)
{
	putText( img, "Test sequenz is running.", Point(20, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);
	putText( img, "Testing evaluation function.", Point(20, 100), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);

	bool eval1Result = 1;
	bool eval2Result = 1;
	bool eval3Result = 1;
	bool eval4Result = 1;
	bool eval5Result = 1;
	bool eval6Result = 1;
	bool eval7Result = 0;
	bool eval8Result = 1;

	vector<vector<Point>> testEmpty;
	vector<vector<Point>> testFull{{Point(1, 1), Point(1, 2), Point(2, 2), Point(2, 1)}, {Point(1, 1), Point(1, 3), Point(2, 3), Point(2, 1)}};

	//Test all possibilties with empty and full vector
	eval1Result = evaluation(testEmpty, 0, 0); //expected 0
	eval2Result = evaluation(testEmpty, 0, 1); //expected 0
	eval3Result = evaluation(testEmpty, 1, 0); //expected 0
	eval4Result = evaluation(testEmpty, 1, 1); //expected 0
	eval5Result = evaluation(testFull, 0, 0); //expected 0
	eval6Result = evaluation(testFull, 0, 1); //expected 0
	eval7Result = evaluation(testFull, 1, 0); //expected 1
	eval8Result = evaluation(testFull, 1, 1); //expected 0

	imshow("Image", img);
	waitKey(2000);

	//Test is passed when only test 7 returns 1
	if(eval1Result == 0 && eval2Result == 0 && eval3Result == 0 && eval4Result == 0 &&
		eval5Result == 0 && eval6Result == 0 && eval7Result == 1 && eval8Result == 0)
	{
		return 1;
	}
	return 0;
}

//Draw results of tests
void drawTestResults(Mat img)
{
	putText( img, "Test sequenz finished.", Point(20, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);

	if(testButtonsResult == 1)
	{
		putText( img, "Test buttons: passed", Point(20, 150), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);
	}
	else
	{
		putText( img, "Test buttons: failed", Point(20, 150), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);
	}

	if(testTrackbarsResult == 1)
	{
		putText( img, "Test trackbars: passed", Point(20, 200), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);
	}
	else
	{
		putText( img, "Test trackbars: failed", Point(20, 200), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);
	}

	if(testCalculateAreaResult == 1)
	{
		putText( img, "Test calculate area function: passed", Point(20, 250), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);
	}
	else
	{
		putText( img, "Test calculate area function: failed", Point(20, 250), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);
	}

	if(testEvaluationResult == 1)
	{
		putText( img, "Test evaluation function: passed", Point(20, 300), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);
	}
	else
	{
		putText( img, "Test evaluation function: failed", Point(20, 300), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);
	}

	putText( img, "Press Esc to continue.", Point(20, 400), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);
	imshow("Image", img);

	while(waitKey(1) != 27){};
}

//Reset test results
void resetTests()
{
	testButtonsResult = 0;
	testTrackbarsResult = 0;
	testCalculateAreaResult = 0;
	testEvaluationResult = 0;
}

//Reset buttons to default
void resetButtons()
{
	buttonRunState = 0;
	buttonEvaluationState = 0;
	buttonContoursState = 0;
	buttonAreaSizeState = 0;
	buttonBoundingBoxState = 0;
	buttonAllWindowsState = 0;
	buttonTestsState = 0;
}
