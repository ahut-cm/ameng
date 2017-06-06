#include <ros/ros.h>               //ros 的头文件
#include <image_transport/image_transport.h>   //image_transport
#include <cv_bridge/cv_bridge.h>              //cv_bridge
#include <sensor_msgs/image_encodings.h>    //图像编码格式
#include <opencv2/imgproc/imgproc.hpp>      //图像处理
#include <opencv2/highgui/highgui.hpp>       //opencv GUI

static const std::string OPENCV_WINDOW = "Image window";   //申明一个GUI 的显示的字符串

class ImageConverter    //申明一个图像转换的类
{
  ros::NodeHandle nh_;        //实例化一个节点
  image_transport::ImageTransport it_;
  image_transport::Subscriber image_sub_;     //订阅节点
  image_transport::Publisher image_pub_;      //发布节点
  
public:
  ImageConverter()
    : it_(nh_)
  {
    // Subscrive to input video feed and publish output video feed
    image_sub_ = it_.subscribe("/usb_cam/image_raw", 1, &ImageConverter::imageCb, this);
    image_pub_ = it_.advertise("/image_converter/output_video", 1);

    cv::namedWindow(OPENCV_WINDOW);
  }

  ~ImageConverter()
  {
    cv::destroyWindow(OPENCV_WINDOW);
  }

  void imageCb(const sensor_msgs::ImageConstPtr& msg)   //回调函数
  {
    cv_bridge::CvImagePtr cv_ptr;  //申明一个CvImagePtr
    try
    {
      cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    }
    catch (cv_bridge::Exception& e)
    {
      ROS_ERROR("cv_bridge exception: %s", e.what());
      return;
    }
//转化为opencv的格式之后就可以对图像进行操作了
    // Draw an example circle on the video stream
Canny(cv_ptr->image,cv_ptr->image,3,3,3);
    // Update GUI Window
    cv::imshow(OPENCV_WINDOW, cv_ptr->image);
    cv::waitKey(3);
    
    // Output modified video stream
    image_pub_.publish(cv_ptr->toImageMsg());
  }
};

int main(int argc, char** argv)
{
  ros::init(argc, argv, "image_converter");
  ImageConverter ic;
  ros::spin();
  return 0;
}
