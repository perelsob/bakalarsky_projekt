#include "ros/ros.h"
   #include "zaciatocnik_navod/AddTwoInts.h"
   #include <cstdlib>
   
   int main(int argc, char **argv)
   {
     ros::init(argc, argv, "add_two_ints_client");
     if (argc != 3)
     {
       ROS_INFO("usage: add_two_ints_client X Y");
       return 1;
     }
   
     ros::NodeHandle n;
     ros::ServiceClient client = n.serviceClient<zaciatocnik_navod::AddTwoInts>("add_two_ints");
     zaciatocnik_navod::AddTwoInts srv;
     srv.request.a = atoll(argv[1]);
     srv.request.b = atoll(argv[2]);
     if (client.call(srv))
     {
       ROS_INFO("Sum: %ld", (long int)srv.response.sum);
     }
     else
     {
       ROS_ERROR("Failed to call service add_two_ints");
       return 1;
     }
   
     return 0;
   }
