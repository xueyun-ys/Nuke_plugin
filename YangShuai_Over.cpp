// Copyright (c) 2009 The Foundry Visionmongers Ltd.  All Rights Reserved.

static const char* const CLASS = "YangShuai_Over";

static const char* const HELP =
  "Do the keymix";

// Standard plug-in include files.

#include "DDImage/Iop.h"
#include "DDImage/NukeWrapper.h"
using namespace DD::Image;
#include "DDImage/Row.h"
#include "DDImage/Tile.h"
#include "DDImage/Knobs.h"

using namespace std;


class YangShuai_Over : public Iop
{
public:

  int minimum_inputs() const { return 2; }
  int maximum_inputs() const { return 2; }

  //! Constructor. Initialize user controls to their default values.

  YangShuai_Over (Node* node) : Iop (node)
  {
  }

  ~YangShuai_Over () {}

  void _validate(bool);
  void _request(int x, int y, int r, int t, ChannelMask channels, int count);

  //! This function does all the work.

  void engine ( int y, int x, int r, ChannelMask channels, Row& out );

  //! Return the name of the class.

  const char* Class() const { return CLASS; }
  const char* node_help() const { return HELP; }

  //! Information to the plug-in manager of DDNewImage/Nuke.

  static const Iop::Description description;

  //...
  void in_channels(int input_number, ChannelSet& channels) const
  {
    // Must turn on the other color channels if any color channels are requested:
    ChannelSet done;
    foreach (z, channels) {
      if (colourIndex(z) < 4) { // it is red, green, or blue
        if (!(done & z)) { // save some time if we already turned this on
          done.addBrothers(z, 4); // add all three to the "done" set
        }
      }
    }
    channels += done; // add the colors to the channels we need
  }
};


/*! This is a function that creates an instance of the operator, and is
   needed for the Iop::Description to work.
 */
static Iop* YangShuai_OverCreate(Node* node)
{
  return new YangShuai_Over(node);
}

/*! The Iop::Description is how NUKE knows what the name of the operator is,
   how to create one, and the menu item to show the user. The menu item may be
   0 if you do not want the operator to be visible.
 */
const Iop::Description YangShuai_Over::description ( CLASS, "Merge/YangShuai_Over",
                                                     YangShuai_OverCreate );


void YangShuai_Over::_validate(bool for_real)
{
  copy_info(); // copy bbox channels etc from input0
  merge_info(1); // merge info from input 1
}

void YangShuai_Over::_request(int x, int y, int r, int t, ChannelMask channels, int count)
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
void YangShuai_Over::engine ( int y, int x, int r,
                              ChannelMask channels, Row& row )
{
  //===========================================================================
  // ChannelSet done;
  // foreach (z, channels) { // visit every channel asked for
  //   if (done & z)
  //     continue;             // skip if we did it as a side-effect of another channel

    // If the channel is not a color, we return it unchanged:
    // if (colourIndex(z) >= 3) {
    //   out.copy(in, z, x, r);
    //   continue;
    // }

    // Find the rgb channels that belong to the set this channel is in.
    // Add them all to "done" so we don't run them a second time:
    // Channel rchan = brother(z, 0);
    // done += rchan;
    // Channel gchan = brother(z, 1);
    // done += gchan;
    // Channel bchan = brother(z, 2);
    // done += bchan;
    // Channel achan = brother(z, 3);
    // done += achan;

    // pixel_engine is called with the channels indicated by in_channels()
    // already filled in. So we can just read them here:
    // const float* rIn = in[rchan] + x;
    // const float* gIn = in[gchan] + x;
    // const float* bIn = in[bchan] + x;
    // const float* aIn = in[achan] + x;

    // We want to write into the channels. This is done with a different
    // call that returns a non-const float* pointer. We must call this
    // *after* getting the in pointers into local variables. This is
    // because in and out may be the same row structure, and calling
    // these may change the pointers from const buffers (such as a cache
    // line) to allocated writable buffers:
    // float* rOut = out.writable(rchan) + x;
    // float* gOut = out.writable(gchan) + x;
    // float* bOut = out.writable(bchan) + x;
    // float* aOut = out.writable(achan) + x;

    //.... Loop across pixels calculating saturation for all three channels
    //.... and setting them in the output row.
  //===========================================================================
  // input 0 row
  row.get(input0(), y, x, r, channels);

  // input 1 row
  Row input1Row(x, r);
  input1Row.get(input1(), y, x, r, channels);

  // const float* alph = row[colourIndex]
  // float tem[];
  // foreach (z, channels)
  // {
  //   if(colourIndex(z) = 3)
  //   {
  //
  //   }
  // }

  foreach ( z, channels ) {
    if(colourIndex(z) < 3)//r,g,b
    {
      const float* input1 = input1Row[z] + x;
      const float* input0  = row[z] + x;
      const float* alp = row[Chan_Alpha] +x;

      float* outptr = row.writable(z) + x;
      const float* end = outptr + (r - x);

      while (outptr < end) {
        *outptr++ = *input0++ * *alp++ + (1 - *alp) * *input1++;
      }
    }

  }
}
