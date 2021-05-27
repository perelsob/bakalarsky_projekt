#include "ros/ros.h"
#include <image_transport/image_transport.h>
#include "cv_bridge/cv_bridge.h"
#include <opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui.hpp>
#include<sensor_msgs/image_encodings.h>
#include <komunikacia/senzor_data.h>
#include <komunikacia/rychlost_pasov.h>
#include <std_msgs/String.h>
//#include <string>


static const std::string OPENCV_WINDOW = "Image window";

	

    
class videoUprava_node
  {
   ros::NodeHandle nh_;
   image_transport::ImageTransport it_;
   image_transport::Subscriber image_sub_;
   image_transport::Publisher image_pub_;
   ros::Subscriber sub1;
   ros::Subscriber sub2;
   ros::Subscriber sub3;
   char text_vzdialenost[25];
   char text_zvuk[25];
   char text_svetlo[25];
   char text_teplota[25];
   char text_pasL[30];
   char text_pasP[30];
   char text_pohyb[30];
   bool vzdialenost_podhranicou;
   const int vzdialenost_dov=40; //40 cm
   
   public:
     videoUprava_node()
       : it_(nh_)
     {
       // Subscrive to input video feed and publish output video feed
       image_sub_ = it_.subscribe("/raspicam_node/image", 1,
         &videoUprava_node::imageCb, this);
       image_pub_ = it_.advertise("Upravenevideo", 1);
       
   sub1 = nh_.subscribe("data_senzorov", 1000,&videoUprava_node::text_senzory_set, this);
   sub2 = nh_.subscribe("pasy_rychlost", 1000,&videoUprava_node::text_pasy_set, this);  
   sub3 = nh_.subscribe("pohyb_stav", 1000,&videoUprava_node::text_pohyb_set, this);       
       
      cv::namedWindow(OPENCV_WINDOW);
   
     
     }
   
     ~videoUprava_node()
     {
 cv::destroyWindow(OPENCV_WINDOW);
  }

  

void text_senzory_set(const komunikacia::senzor_data::ConstPtr& msg)
	{
	if ((msg->vzdialenost) < vzdialenost_dov)
		vzdialenost_podhranicou=true;
	else
		vzdialenost_podhranicou=false;
		
	sprintf(text_vzdialenost,"vzdialenost: %dcm",msg->vzdialenost);
	sprintf(text_zvuk,   "intenzita zvuku: %d",msg->zvuk);
	sprintf(text_svetlo, "intenzita svetla:  %d",msg->svetlo);
	sprintf(text_teplota,"teplota:  %.2f C",msg->teplota);
	}

void text_pasy_set(const komunikacia::rychlost_pasov::ConstPtr& msg)	

	{	
	sprintf(text_pasL,"rychlost pasL:  %.2fcm/s",msg->pas_l);
	sprintf(text_pasP,"rychlost pasP:  %.2fcm/s",msg->pas_p);
	}

void text_pohyb_set(const std_msgs::String::ConstPtr& msg)	
	{
	
	sprintf(text_pohyb,"pohyb: %s",msg->data.c_str());
	}

 void imageCb(const sensor_msgs::ImageConstPtr& msg) //*
    {
      cv_bridge::CvImagePtr cv_ptr;
      try
       {
         cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
       }
       catch (cv_bridge::Exception& e)
       {
         ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
       }
 
// Draw an example circle on the video stream
       //if (cv_ptr->image.rows > 60 && cv_ptr->image.cols > 60)
 	 //	cv::circle(cv_ptr->image, cv::Point(50, 50), 10, CV_RGB(255,0,0));


	
 	
 	putText(cv_ptr->image, "Bakalarsky projekt", cv::Point(445,30), cv::FONT_HERSHEY_DUPLEX, 1.2, cv::Scalar(0,200,0), 1.8);
 	cv::circle(cv_ptr->image, cv::Point(1245, 93), 4, CV_RGB(0,200,0));
        
        if (vzdialenost_podhranicou==true)
        	putText(cv_ptr->image, text_vzdialenost, cv::Point(5,50), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0,0,200), 1.8);
        else
        	putText(cv_ptr->image, text_vzdialenost, cv::Point(5,50), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0,200,0), 1.8);
        
        
        
        putText(cv_ptr->image, text_svetlo, cv::Point(920,50), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0,200,0), 1.8);
        putText(cv_ptr->image, text_zvuk, cv::Point(920,80), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0,200,0), 1.8);
        putText(cv_ptr->image, text_teplota, cv::Point(990,110), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0,200,0), 1.8);
        
        putText(cv_ptr->image, text_pasL, cv::Point(5,950), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0,200,0), 1.8);
        putText(cv_ptr->image, text_pasP, cv::Point(830,950), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0,200,0), 1.8);
        putText(cv_ptr->image, text_pohyb, cv::Point(535,950), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0,200,0), 1.8);
       //putText(cv_ptr->image, "Pokus2", cv::Point(5,55), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0,0,200), 2);
       
       cv::resize(cv_ptr->image, cv_ptr->image, cv::Size(cv_ptr->image.cols * 0.9,cv_ptr->image.rows * 0.9), 0, 0, CV_INTER_LINEAR);
       
       // Update GUI Window
       cv::imshow(OPENCV_WINDOW, cv_ptr->image);
       cv::waitKey(3);
 
 	// Output modified video stream
 	image_pub_.publish(cv_ptr->toImageMsg());
 
   }
};


	

int main(int argc, char** argv)
   {
     ros::init(argc, argv, "videoUprava_node");
   ros::NodeHandle n;
   videoUprava_node vidup;
  // ros::Subscriber sub = n.subscribe("Data_senzorov", 1000,&ImageConverter::text_set, &ic);
     ros::spin();
     return 0;
   }



