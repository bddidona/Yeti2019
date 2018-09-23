#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{

  /* 
    cv::Mat snow, origimage;
    origimage = cv::imread("pic.jpg");
    cv::GaussianBlur(origimage,snow,cv::Size(5,5),0,0);
     */

    cv::VideoCapture cap("singleI.m4v");
    if(!cap.isOpened()){
    std::cout << "Error opening video stream or file" << std::endl;
    return -1;
  }
  cv::namedWindow( "Snow", cv::WINDOW_NORMAL );
  cv::resizeWindow("Snow",1280,720);
    while(1)
    {

    
    cv::Mat snow, blurimage, origimage;
    cap >> origimage;
    if (origimage.empty())
      break;
    
    cv::GaussianBlur(origimage,snow,cv::Size(5,5),0,0);
    blurimage = snow;
    cv::inRange(snow, cv::Scalar(150,150,150), cv::Scalar(255,255,255), snow);
    cv::Mat ground_mask = cv::Mat::zeros(snow.size(), snow.type());
    cv::rectangle(ground_mask, cv::Rect(0,2.5*ground_mask.rows/4,ground_mask.cols,ground_mask.rows/4),cv::Scalar(255,255,255),CV_FILLED);
    cv::bitwise_and(snow,ground_mask,snow);



    int thresh = 100;
    int k_erode_size = 10;
    int k_dialate_size = 25;
    cv::RNG rng(12345);

    /* cv::createTrackbar("thresh", "Snow", &thresh, 255);
    cv::createTrackbar("erode", "Snow", &k_erode_size, 255);
    cv::createTrackbar("dialate", "Snow", &k_dialate_size, 255);
    */
   
    cv::Mat kernel_erode = cv::Mat::ones(k_erode_size, k_erode_size, CV_32F)/ (float)(k_erode_size*k_erode_size);
    cv::Mat kernel_dialate = cv::Mat::ones(k_dialate_size, k_dialate_size, CV_32F)/ (float)(k_dialate_size*k_dialate_size);
    cv::erode(snow, snow,kernel_erode);
    cv::dilate(snow,snow,kernel_dialate);
    cv::Mat threshold_output;
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;

    cv::threshold( snow, threshold_output, thresh, 255, cv::THRESH_BINARY );
    /// Find contours
    cv::findContours( threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );

  /// Approximate contours to polygons + get bounding rects and circles
  std::vector<std::vector<cv::Point> > contours_poly( contours.size() );
  std::vector<cv::Rect> boundRect( contours.size() );
  std::vector<cv::Point2f>center( contours.size() );

  for( int i = 0; i < contours.size(); i++ )
     { cv::approxPolyDP( cv::Mat(contours[i]), contours_poly[i], 3, true );
       boundRect[i] = cv::boundingRect( cv::Mat(contours_poly[i]) );
     }


  /// Draw polygonal contour + bonding rects + circles
  cv::Mat drawing = cv::Mat::zeros( threshold_output.size(), CV_8UC3 );
  for( int i = 0; i< contours.size(); i++ )
     {
       cv::Scalar color = cv::Scalar(255,0,255);
       drawContours( drawing, contours_poly, i, color, CV_FILLED, 8, std::vector<cv::Vec4i>(), 0, cv::Point() );
       //rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
     }

  cv::namedWindow( "Snow", cv::WINDOW_NORMAL );
  cv::resizeWindow("Snow",1280,720);
  cv::imshow( "Snow", (drawing | blurimage) );
  char c=(char)cv::waitKey(25);
    if(c==27)
      break;
    }
    return(0);
}