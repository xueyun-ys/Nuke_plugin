// Copyright (c) 2009 The Foundry Visionmongers Ltd.  All Rights Reserved.

static const char* const CLASS = "YangShuai_Median";

static const char* const HELP =
  "Does a Median filter";

// Standard plug-in include files.

#include "DDImage/Iop.h"
#include "DDImage/NukeWrapper.h"
using namespace DD::Image;
#include "DDImage/Row.h"
#include "DDImage/Tile.h"
#include "DDImage/Knobs.h"

#include <stdio.h>
#include <stdlib.h>

using namespace std;

class YangShuai_Median : public Iop
{

  int _size;

public:

  int maximum_inputs() const { return 1; }
  int minimum_inputs() const { return 1; }

  //! Constructor. Initialize user controls to their default values.

  YangShuai_Median (Node* node) : Iop (node)
  {
    _size = 10;
  }

  ~YangShuai_Median () {}

  void _validate(bool);
  void _request(int x, int y, int r, int t, ChannelMask channels, int count);

  //! This function does all the work.

  void engine ( int y, int x, int r, ChannelMask channels, Row& out );

  //! Return the name of the class.

  const char* Class() const { return CLASS; }
  const char* node_help() const { return HELP; }

  //! Information to the plug-in manager of DDNewImage/Nuke.

  static const Iop::Description description;

  //Knobs
  virtual void knobs(Knob_Callback);
};


/*! This is a function that creates an instance of the operator, and is
   needed for the Iop::Description to work.
 */
static Iop* YangShuai_MedianCreate(Node* node)
{
  return new YangShuai_Median(node);
}

/*! The Iop::Description is how NUKE knows what the name of the operator is,
   how to create one, and the menu item to show the user. The menu item may be
   0 if you do not want the operator to be visible.
 */
const Iop::Description YangShuai_Median::description ( CLASS, "Merge/YangShuai_Median",
                                                     YangShuai_MedianCreate );


void YangShuai_Median::_validate(bool for_real)
{
  copy_info(); // copy bbox channels etc from input0, which will validate it.
  info_.pad( _size);

}

void YangShuai_Median::_request(int x, int y, int r, int t, ChannelMask channels, int count)
{
  // request extra pixels around the input
  input(0)->request( x - _size , y - _size , r + _size, t + _size, channels, count );
}

/*
Method for finding the median value
*/

void swap(float arr[], int a, int b){
    float tmp = arr[a];
    arr[a] = arr[b];
    arr[b] = tmp;
}

void quick_select(float arr[], int k, int s, int e){
    if(s > e || k < s || k > e){
        printf("invalid array range\n");
        return;
    }

    int i, j;
    float pivot = arr[s];
    if(s <= e){
        i = s;
        j = e;
        for(;;){
            while(arr[j] >= pivot && i<j){j--;}
            while(arr[i] <= pivot && i<j){i++;}
            if(i<j)
                swap(arr, i, j);
            else
                break;
        }
        //refresh pivot
        swap(arr, i, s);

        //recursion
        if(k<=i)
            quick_select(arr, k, s, i-1);
        else if(k>=i+1)
            quick_select(arr, k, i+1, e);
    }
}

//copy array
void copy_array(int arr[], const int barr[], int n){
    int i = 0;
    for(i=0; i<n; i++)
        arr[i] = barr[i];
}

//knobs for interface
void YangShuai_Median::knobs(Knob_Callback f)
{
  Int_knob(f, &_size, IRange(1, 30), "medianfilter_size");
}


/*! For each line in the area passed to request(), this will be called. It must
   calculate the image data for a region at vertical position y, and between
   horizontal positions x and r, and write it to the passed row
   structure. Usually this works by asking the input for data, and modifying
   it.

 */
void YangShuai_Median::engine ( int y, int x, int r,
                              ChannelMask channels, Row& row )
{

  // make a tile for current line with padding arond for the blur
  Tile tile( input0(), x - _size , y - _size , r + _size, y + _size , channels);
  if ( aborted() ) {
    std::cerr << "Aborted!";
    return;
  }


  foreach ( z, channels ) {
    float* outptr = row.writable(z) + x;
    for( int cur = x ; cur < r; cur++ ) {
      float value = 0;
      //float div = 0;
      int num = 0;
      float tem[_size*_size*4];

      if ( intersect( tile.channels(), z ) ) {
        // a simple box blur
        for ( int px = -_size; px < _size; px++ ) {
          for ( int py = -_size; py < _size; py++ ) {
            tem[num] = tile[z][ tile.clampy(y + py)][ tile.clampx(cur + px) ];
            //value += tile[z][ tile.clampy(y + py)][ tile.clampx(cur + px) ];
            num++;
          }
        }
        //if ( div )
          //value /= div;
          //==============================================================
          int n = _size*_size*4;

          //int b[n];
          //copy_array(b, tem, n);
          int m = _size*_size*2-1;

          //求解中位数
          quick_select(tem, m, 0, n-1);
          *outptr++ = tem[m];
          //==============================================================
      }
      //*outptr++ = value;
    }
  }
}
