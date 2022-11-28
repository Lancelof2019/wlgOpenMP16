#include "../headerspace/WatershedAlg.h"
#include<vector>
using namespace cv;
#define NUMSIZE 8
#define NSIZE 4

struct Compare{

   bool operator()(int* para1, int* para2) {
        return para1[0] < para2[0];
    }

};

Mat WatershedAlg::watershed(Array2D<int> &image, Array2D<int>& markers,Mat &duplImage,int &rows,int &cols,Array2D<bool> &inprioq,Array2D<int>& markerMap,Array2D<int>&temp,Array2D<int>& nextSet){



priority_queue<int*,vector<int*>,Compare>prioq;


       int msize=0;
  
       int dx[NUMSIZE]={-1, 1, 0, 0, -1, -1, 1, 1};
       int dy[NUMSIZE]={0, 0, -1, 1, -1,  1, 1, -1};

        // Put markers in priority queue
        int id = 1;
        Mat testDuplicate;
        duplImage.copyTo(testDuplicate);
        Mat dstImage(duplImage.rows,duplImage.cols,CV_8UC3,Scalar::all(0));


int tempcounter=0;

   for(int y=0;y<rows;y++){

      for(int z=0;z<cols;z++){
 
           if(markers(y,z)==2){    
            markerMap(y,z) = id;
            msize++;
             for(int i = 0; i < 4; i++) {

                int newX =y + dx[i];
                int newY =z + dy[i];
                if(newX < 0 || newY < 0 || newX >= rows || newY >= cols) {
                    continue;
                }

	       temp(tempcounter,0)=image(newX,newY);
               temp(tempcounter,1)=newX;
               temp(tempcounter,2)=newY;
               prioq.push(temp(tempcounter));
	       tempcounter++;

                
             }

             id++;
           }
        }
   }



         cv::Vec3b colors[msize+1];
         #pragma omp parallel for
         for(int i = 0; i <= msize; i++) {
           Vec3b vecColor;
           vecColor[0] = rand()%255+0;
           vecColor[1] = rand()%255+1;
           vecColor[2] = rand()%255+2;          
           colors[i]=vecColor;
        }

int ptcounter=0;
        while(!prioq.empty()) {
             int curpoint[3]={prioq.top()[0],prioq.top()[1],prioq.top()[2]};
             prioq.pop();
             int CrtX=curpoint[1];
             int CrtY=curpoint[2];
  
            bool canLabel = true;
            int neighboursLabel = 0;
            for(int i = 0; i < 8; i++) {
            
   
              int nextX = curpoint[1] + dx[i];
              int nextY = curpoint[2] + dy[i];
             

            
              if(nextX < 0 || nextY < 0 || nextX >= rows || nextY >= cols) {
                    continue;
                }

		 nextSet(ptcounter,0)=image(nextX,nextY);

                 nextSet(ptcounter,1)=nextX;
	        nextSet(ptcounter,2)=nextY;

                if(markerMap(nextX,nextY) != 0 &&  nextSet(ptcounter,0)!= 0){
                    if(neighboursLabel == 0) {
                        neighboursLabel = markerMap(nextX,nextY);//using id value,all strats from their closest neighbour
                    } else {//this part tells us that if there is two points at the boundary to see if they are in the same classification,
                    //two classification there is no merge
                        if(markerMap(nextX,nextY) != neighboursLabel ) {
                            canLabel = false;
                        }
                    }
                } else {
                    if(inprioq(nextX,nextY) == false) {
                        inprioq(nextX,nextY) = true;//aviod duplicate point is chosen,point does not exist in marker or background
                        prioq.push(nextSet(ptcounter));
                        //the most important expending step,//only the point whose pixel value is 0 in Markermap
                    }
                }
		ptcounter++;

            }

            if(canLabel&&image(CrtX,CrtY)!=0) {
                
                markerMap(CrtX,CrtY) = neighboursLabel;//in this way it tells us that the points in same region share the same id or label
               duplImage.at<Vec3b>(CrtX,CrtY)=colors[ markerMap(CrtX,CrtY) ];
         
                
            }
         
        }

       cv::addWeighted(duplImage,0.4,testDuplicate,0.6,0,dstImage);

    
        duplImage.release();
        testDuplicate.release();
        return dstImage;
    }
