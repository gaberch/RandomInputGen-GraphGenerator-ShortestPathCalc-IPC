#include <algorithm>
#include <cmath>
#include <csignal>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>







// int flag = 0;
int num_tries = 0;
bool intersection_occured = false;
const unsigned min_num_streets = 2;
const unsigned min_line_segments = 1;
const unsigned min_wait = 5;

// std::ofstream myfile;


unsigned get_random_num(int min, int max){
// int get_random_num(int min, int max, int &flag){

   std::ifstream urandom("/dev/urandom");

   if (urandom.fail()) {
     std::cerr << "Error: cannot open /dev/urandom\n";
     exit(EXIT_FAILURE);;
   }

   char random_num = 'a';
   urandom.read(&random_num,1);


   urandom.close();

   return min + unsigned (random_num) % (max - min + 1);
   // return int (random_num);

}

// Given three colinear points (x1,y1), (x_mid, y_mid), (x2,y2), the function checks if
// point (x_mid, y_mid) lies on line segment (x1,y1) - (x2,y2)
bool on_segment(int x1,int  y1,int x_mid,int y_mid,int x2,int y2){
    return(std::max(x1, x2) >= x_mid && x_mid >= std::min(x1, x2) && std::max(y1, y2) >= y_mid && y_mid >= std::min(y1, y2));
}

int orientation(int x1,int y1,int x_mid,int y_mid,int x2, int y2){
    int val = (y_mid - y1) * (x2 - x_mid) - (x_mid-x1) * (y2 - y_mid);

    if (val == 0){ return 0;}
    if (val > 0){return 1;}
    return 2;
}

bool do_intersect(int x1,int y1,int x2,int y2,int x3,int y3,int x4,int y4){
   int o1 = orientation(x1,y1,x2,y2,x3,y3);
   int o2 = orientation(x1,y1,x2,y2,x4,y4);
   int o3 = orientation(x3,y3,x4,y4,x1,y1);
   int o4 = orientation(x3,y3,x4,y4,x2,y2);

   // General intersection case
   if (o1 != o2 && o3 != o4){
     return true;
   }

   // # Special Cases

   // L1P1, L1P2 and L2P1 are collinear and L2P1 lies on segment L1
   if (o1 == 0 && on_segment(x1, y1, x3, y3, x2, y2)){
      return true;
   }

   // L1P1, L1P2 and L2P2 are collinear and L2P2 lies on segment L1
   if (o2 == 0 && on_segment(x1, y1, x4, y4, x2, y2)){
      return true;
   }

   // L2P1, L2P2 and L1P1 are collinear and L1P1 lies on segment L2
   if (o3 == 0 && on_segment(x3, y3, x1, y1, x4, y4)){
      return true;
   }

   // L2P1, L2P2 and L1P2 are collinear and L1P2 lies on segment L2
   if (o4 == 0 && on_segment(x3, y3, x2, y2, x4, y4)){
     return true;
   }

   return false;
}

bool on_line(int x1,int y1,int x_mid,int y_mid,int x2,int y2){
    double crossproduct = 1.0* (y_mid - y1) * (x2 - x1) - (x_mid - x1) * (y2 - y1);
    double dotproduct = 1.0*(x_mid - x1) * (x2 - x1) + (y_mid - y1) * (y2 - y1);
    double squaredlengthba = 1.0*(x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
    // compare versus epsilon for floating point values, or != 0 if using integers

    return (!(std::abs(crossproduct) > 0.00001) && !(dotproduct < 0) && !(dotproduct > squaredlengthba) );
    // if (std::abs(crossproduct) > 0.00001)
    //     {return false;}
    //
    //
    // if (dotproduct < 0)
    //     {return false;}
    //
    //
    // if (dotproduct > squaredlengthba)
    //     {return false;}
    //
    // return true;
}

bool overlap_check(int x1,int y1,int x2,int y2,int x3,int y3,int x4,int y4){

   // double xnum = ((1.0 * x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4));
   double xden = ((1.0* x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4));
   // double ynum = (1.0 * x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4);
   double yden = (1.0 * x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

   return (xden == 0 || yden == 0);


   // if (xden == 0 || yden == 0){
   //    return true;
   // }
   // return false;

}


double segment_length(int x1, int y1, int x2, int y2){
   return 1.0 * sqrt(pow((x2-x1),2)+pow((y2-y1),2));
}











class Point
{
// private:


public:
   int x;
   int y;
   int k;

   explicit Point(int k_limit){
      k = k_limit;
      x = 0;
      y = 0;
   }

   void generate_pt(){

      std::ifstream urandom("/dev/urandom");

      if (urandom.fail()) {
        std::cerr << "Error: cannot open /dev/urandom\n";
        exit(EXIT_FAILURE);
      }

      char random_num = 'a';
      urandom.read(&random_num,1);

      x = int (random_num) % (k + 1);
      urandom.read(&random_num,1);
      y = int (random_num) % (k + 1);
      urandom.close();
   }
};

class Street
{

private:
   // bool intersect ()
   std::vector <int> x_pts;
   std::vector <int> y_pts;

public:
   std::string name;
   std::string points;

   // organize array like so
   /*
   [x1][y1]          x = 0,0, y = 1,0
   [x2][y2]          x = 1,0, y = 1,1
   [x3][y3]          x = 2,0, y = 1,2
   */
   std::vector<std::vector<int>> pts;


   void init(int input[], std::vector <Street> all_streets, unsigned num_streets)
   {
      name += "a";
      for (unsigned i = 0; i < all_streets.size(); i++){
         name += "b";
      }
      name += " street";
      unsigned max_line_segments = input[1];
      unsigned line_segments = get_random_num (min_line_segments, max_line_segments);
      Point pt_obj (input[3]);
      pts.push_back(x_pts);
      pts.push_back(y_pts);

      // std::cout << "K value: " << pt_obj.k << "\n";
      // std::cout << "Number of line segments: " << line_segments << "\n";
      unsigned count = 0;
      int temp = 0;

      /*check that each new point created does not do the following:
         -create a line segment of length 0
         -create a pt on a previously created line segment
         -create a line segment that intersects with a previous line segment
      */
      while (count < line_segments){
      // for (int count = 0; count < line_segments; count++ ){
         temp = num_tries;
         if (num_tries == 25){
            std::cerr << "Error: Rgen failed to generate valid input for 25 simultaneous attempts\n";
            exit(EXIT_FAILURE);
         }


         if (count == 0 && pts[0].empty()){
            // First line segment
            pt_obj.generate_pt();
            pts[0].push_back(pt_obj.x);
            pts[1].push_back(pt_obj.y);
            pt_obj.generate_pt();
            pts[0].push_back(pt_obj.x);
            pts[1].push_back(pt_obj.y);
            // std::cout << "First push_back occured \n";
         }
         else if (pts[0].size() == count + 1){
            pt_obj.generate_pt();
            pts[0].push_back(pt_obj.x);
            pts[1].push_back(pt_obj.y);
            // std::cout << "Next push_back occured \n";
         }
         else {
            pt_obj.generate_pt();
            pts[0][count + 1] = pt_obj.x;
            pts[1][count+1] = pt_obj.y;
            // std::cout << "Count = " << count << "pts array size is " << pts[0].size() << ", Array elements replaced with new points \n";
         }


         bool self_segment_issue = false;

         if (segment_length(pts[0][count],pts[1][count],pts[0][count+1],pts[1][count+1]) == 0)
         {
            num_tries++;
            // myfile << "0 length segment created\n";
         }
         else if (count != 0){

            for (unsigned i = 0; i < pts[0].size() - 2; i++){
               // std:: cout << "Pt (" << pts[0][count+1] << "," << pts[1][count+1] << ") is checked against segment (" << pts[0][i] << "," << pts[1][i] << ") - (" << pts[0][i+1] << "," << pts[1][i+1] << ")\n";
               // if (i < pts[0].size() - 3)
                  // std::cout << "Segment ("<< pts[0][count] << "," << pts[1][count] << ") - (" << pts[0][count+1] << "," << pts[1][count+1] << ") is checked against segment (" << pts[0][i] << "," << pts[1][i] << ") - (" << pts[0][i+1] << "," << pts[1][i+1] << ")\n";

               if (on_line(pts[0][i],pts[1][i],pts[0][count+1],pts[1][count+1],pts[0][i+1],pts[1][i+1])){
                  // myfile  << "Pt " << pts[0][count+1] << "," << pts[1][count+1] << " is on segment " << pts[0][i] << "," << pts[1][i] << " " << pts[0][i+1] << "," << pts[1][i+1] << "\n";
                  num_tries++;
                  self_segment_issue = true;
                  break;
               }

               if (do_intersect(pts[0][i],pts[1][i],pts[0][i+1],pts[1][i+1],pts[0][count],pts[1][count],pts[0][count+1],pts[1][count+1]) && i < pts[0].size() - 3){
                  // std:: cout << "pts array size: " << pts[0].size() << ", i = " << i << std::endl;
                  // myfile  << "Segment "<< pts[0][count] << "," << pts[1][count] << " " << pts[0][count+1] << "," << pts[1][count+1] << " is intersecting segment " << pts[0][i] << "," << pts[1][i] << " " << pts[0][i+1] << "," << pts[1][i+1] << "\n";
                  num_tries++;
                  self_segment_issue = true;
                  break;
               }



            }
         }

         if (!self_segment_issue){
            bool for_loop_flag = false;
            for (unsigned i = 0; i < all_streets.size(); i++){
               for (unsigned j = 0; j < all_streets[i].pts[0].size() - 1; j++){
                  //needs to be tested
                  if (overlap_check(all_streets[i].pts[0][j],all_streets[i].pts[1][j],all_streets[i].pts[0][j+1],all_streets[i].pts[1][j+1],pts[0][count],pts[1][count],pts[0][count+1],pts[1][count+1])){
                     // myfile  << "overlap with other streets detected\n";
                     num_tries++;
                     for_loop_flag = true;
                     break;
                  }
                  //needs to be tested

                  if (all_streets.size() == num_streets - 1 && count == line_segments - 1 && !intersection_occured && i == all_streets.size() - 1 && j == all_streets[i].pts[0].size() - 2){
                     // myfile  << "last segment didn't intersect any street\n";
                     num_tries++;
                     for_loop_flag = true;
                     break;
                  }

                  if (do_intersect(all_streets[i].pts[0][j],all_streets[i].pts[1][j],all_streets[i].pts[0][j+1],all_streets[i].pts[1][j+1],pts[0][count],pts[1][count],pts[0][count+1],pts[1][count+1])){
                     intersection_occured = true;
                     // int x1 = all_streets[i].pts[0][j];
                     // int y1 = all_streets[i].pts[1][j];
                     // int x2 = all_streets[i].pts[0][j+1];
                     // int y2 = all_streets[i].pts[1][j+1];
                     // int x3 = pts[0][count];
                     // int y3 = pts[1][count];
                     // int x4 = pts[0][count+1];
                     // int y4 = pts[1][count+1];
                     // myfile  << "Intersection occured between segment (" << x1 << "," << y1 << ") - (" << x2 << "," << y2 << ") and segment (" << x3 << "," << y3 << ") - (" << x4 << "," << y4 << ")\n";
                  }
               }
               if (for_loop_flag){
                  break;
               }
            }
         }

         if (temp == num_tries){
            count++;
         }


      }

      // std::cout << "New street created - name is " << name << std::endl;
      // for (int i = 0; i < pts[0].size();i++)
      //    std::cout << "(" << pts[0][i] << "," << pts[1][i] << ")" << " " ;
      // std::cout << std::endl;

      // std::cout << "Segment lengths are: ";
      // for (int i = 1; i < pts[0].size();i++)
      //    std::cout << segment_length(pts[0][i-1],pts[1][i-1],pts[0][i],pts[1][i]) << " ";
      // std::cout << std::endl;
      // std::cout << "num_tries = " << num_tries << std::endl;


   }


};









int main (int argc, char **argv) {
   std::string strcmd [4];
   int cli_commands [] = {10,5,5,20};
   int cmd;
   // int flag = 0;

   while((cmd = getopt (argc, argv, "s:n:l:c:")) != -1){
      char *end;
      switch (cmd)
      {
         case 's':
            strcmd[0] = optarg;
            cli_commands[0] = strtol(strcmd[0].c_str(),&end,10);
            break;
         case 'n':
            strcmd[1] = optarg;
            cli_commands[1] = strtol(strcmd[1].c_str(),&end,10);
            break;
         case 'l':
            strcmd[2] = optarg;
            cli_commands[2] = strtol(strcmd[2].c_str(),&end,10);

            break;
         case 'c':
            strcmd[3] = optarg;
            cli_commands[3] = strtol(strcmd[3].c_str(),&end,10);
            break;

      }
   }





   /*
   -s k — where k is an integer ≥ 2. The number of streets should be a random integer in [2, k].
   If this option is not specified, you should use a default of k = 10; that is, the number of streets
   should be a random integer in [2, 10].
   */




   // int regen_num = 1;
   // myfile.open("rgen_talking.txt");
   while (true){
      num_tries = 0;
      intersection_occured = false;
      unsigned num_streets = get_random_num(min_num_streets, cli_commands[0]);
      // std::cout << "Number of streets to create: " << num_streets << "\n";
      std::vector<Street> all_streets;

      // std:: cout << "overlap check of (1,0)(2,0) and (0,0) and (3,0): " << overlap_check(1,0,2,0,0,0,3,0) << std::endl;
      // std:: cout << "intersection check of (1,0)(2,0) and (0,0) and (3,0): " << do_intersect(1,0,2,0,0,0,3,0) << std::endl;


      /*
      -n k — where k is an integer ≥ 1. The number of line-segments in each street should be a
      random integer in [1, k]. Default: k = 5.
      */
      // unsigned line_segments;
      // line_segments = get_random_num(min_line_segments, cli_commands[1]);



      // myfile << std::endl;
      // myfile << "Regen set: " << regen_num << std::endl;





      for (unsigned i = 0; i < num_streets; i++){
         Street new_street;
         new_street.init(cli_commands, all_streets, num_streets);

         all_streets.push_back(new_street);
      }



      /*
      -l k — where k is an integer ≥ 5. Your process should wait a random number w seconds,
      where w is in [5, k] before generating the next (random) input. Default: k = 5.
      */
      unsigned wait_time = get_random_num(min_wait, cli_commands[2]);
      // wait_time = 30;





      std::string output;

      //write to a file - for debugging purposes



      for (unsigned i = 0; i < num_streets; i++){
         output = "a \"" + all_streets[i].name + "\"";
         for (unsigned j = 0; j < all_streets[i].pts[0].size(); j++){
            output += " (" + std::to_string(all_streets[i].pts[0][j]) + "," + std::to_string(all_streets[i].pts[1][j]) + ")";
         }
         std::cout << output << std::endl;

         // myfile << output << std::endl;

      }
      // myfile << std:: endl;

      // regen_num++;




      std::cout << "g" << std::endl;
      sleep(wait_time);

      for (unsigned i = 0; i < num_streets; i++){
         output = "r \"" + all_streets[i].name + "\"";
         std::cout << output << std::endl;
      }
      /*
      -c k — where k is an integer ≥ 1. Your process should generate (x, y) coordinates such that
      every x and y value is in the range [−k, k]. For example, if k = 15, all of your coordinate values
      should be integers between −15 and 15. Default: k = 20.
      */






   }
   // myfile.close();

   return 0;
}
