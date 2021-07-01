// Copyright (c) 2009 The Foundry Visionmongers Ltd.  All Rights Reserved.

static const char* const CLASS = "YangShuai_ColorDifference";

static const char* const HELP =
  "ColorDifference";

// Standard plug-in include files.

#include "DDImage/Iop.h"
#include "DDImage/NukeWrapper.h"
using namespace DD::Image;
#include "DDImage/Row.h"
#include "DDImage/Tile.h"
#include "DDImage/Knobs.h"

using namespace std;

class YangShuai_ColorDifference : public Iop
{
public:

  int minimum_inputs() const { return 2; }
  int maximum_inputs() const { return 2; }

  //! Constructor. Initialize user controls to their default values.

  YangShuai_ColorDifference (Node* node) : Iop (node)
  {
  }

  ~YangShuai_ColorDifference () {}

  void _validate(bool);
  void _request(int x, int y, int r, int t, ChannelMask channels, int count);

  //! This function does all the work.

  void engine ( int y, int x, int r, ChannelMask channels, Row& out );

  //! Return the name of the class.

  const char* Class() const { return CLASS; }
  const char* node_help() const { return HELP; }

  //! Information to the plug-in manager of DDNewImage/Nuke.

  static const Iop::Description description;

};


/*! This is a function that creates an instance of the operator, and is
   needed for the Iop::Description to work.
 */
static Iop* YangShuai_ColorDifferenceCreate(Node* node)
{
  return new YangShuai_ColorDifference(node);
}

/*! The Iop::Description is how NUKE knows what the name of the operator is,
   how to create one, and the menu item to show the user. The menu item may be
   0 if you do not want the operator to be visible.
 */
const Iop::Description YangShuai_ColorDifference::description ( CLASS, "Merge/YangShuai_ColorDifference",
                                                     YangShuai_ColorDifferenceCreate );


void YangShuai_ColorDifference::_validate(bool for_real)
{
  copy_info(); // copy bbox channels etc from input0
  merge_info(1); // merge info from input 1
}

void YangShuai_ColorDifference::_request(int x, int y, int r, int t, ChannelMask channels, int count)
{
  // request from input 0 and input 1
  input(0)->request( x, y, r, t, channels, count );
  input(1)->request( x, y, r, t, channels, count );
}


/*! For each line in the area passed to request(), this will be called. It must
   calculate the image data for a region at vertical position y, and between
   horizontal positions x and r, and write it to the passed row
   structure. Usually this works by asking the input for data, and modifying
   it.

 */
void YangShuai_ColorDifference::engine ( int y, int x, int r,
                              ChannelMask channels, Row& row )
{
  // input 0 row
  row.get(input0(), y, x, r, channels);

  // input 1 row
  Row input1Row(x, r);
  input1Row.get(input1(), y, x, r, channels);

  foreach ( z, channels ) {
    if(colourIndex(z) < 2)
    {
      const float* input1 = input1Row[z] + x;
      const float* input0 = row[z] + x;
      const float* inputr  = row[Chan_Red] + x;
      const float* inputg  = row[Chan_Green] + x;
      const float* inputb  = row[Chan_Blue] + x;
      //const float* newb = row[Chan_Blue] + x;
      //const float* inputa  = row[Chan_Alpha] + x;
      float* outptr = row.writable(z) + x;
      const float* end = outptr + (r - x);

      while (outptr < end) {
        //float newblue;//*newblue;
        float mat = 0;
        float imat = 0;
        // if(*inputb > *inputg)
        // {
        //   newblue = *inputg;
        // }
        // else
        // {
        //   newblue = *inputb;
        // }
        mat = *inputb++ - max(*inputr++, *inputg++);
        if(mat<0)
          {mat = 0;}
        //if(mat>1)
          //{mat = 1;}
        imat = 1 - mat;
        //*outptr++ = vec4(( *input0++ * mat + (1-mat) * *input1++).rgb, 0) ;//mat;//
        *outptr++ = *input0++ * imat + mat * *input1++;//*input0++;//mat;

        // *inputb++;
        // *inputr++;
        // *inputg++;
      }
    }
    else if(colourIndex(z) ==2)//> 1&&colourIndex(z)<3)
    {
      const float* input11 = input1Row[z] + x;
      const float* input00 = row[z] + x;
      const float* inputrr  = row[Chan_Red] + x;
      const float* inputgg  = row[Chan_Green] + x;
      const float* inputbb  = row[Chan_Blue] + x;
      //const float* newb = row[Chan_Blue] + x;
      //const float* inputa  = row[Chan_Alpha] + x;
      float* outptr = row.writable(z) + x;
      const float* end = outptr + (r - x);

      while (outptr < end) {
        float newblue;//*newblue;
        float mat = 0;
        float imat = 0;
        if(*inputbb > *inputgg)
        {
          newblue = *inputgg;
        }
        else
        {
          newblue = *inputbb;
        }
        mat = *inputbb++ - max(*inputrr++, *inputgg++);
        if(mat<0)
          {mat = 0;}
        //if(mat>1)
          //{mat = 1;}
        imat = 1 - mat;
        //*outptr++ = vec4(( *input0++ * mat + (1-mat) * *input1++).rgb, 0) ;//mat;//
        *outptr++ = newblue * imat + mat * *input11++;//*input0++;//mat;

        //inputb++;//*inputb++?
        //inputr++;
        //inputg++;
      }
    }
    // else
    // {
    //     const float* inptr = in[z] + x;
    //     const float* END = inptr + (r - x);
    //     float* outptr = out.writable(z) + x;
    //     while (inptr < END)
    //       *outptr++ = *inptr++;
    // }
  }
}
