/*-----------------------------------------------------------------
LOG
    GEM - Graphics Environment for Multimedia

    Snap a pix of the render buffer

    Copyright (c) 1997-1999 Mark Danks. mark@danks.org
    Copyright (c) Günther Geiger. geiger@epy.co.at
    Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::für::umläute. IEM. zmoelnig@iem.kug.ac.at
    For information on usage and redistribution, and for a DISCLAIMER OF ALL
    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.

-----------------------------------------------------------------*/

#ifndef INCLUDE_PIX_SNAP_H_
#define INCLUDE_PIX_SNAP_H_

#include "Base/GemBase.h"
#include "Base/GemPixUtil.h"
#include "FreeImage.h"

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    fux_glitch
    
    Snaps a pix of the render buffer
    
KEYWORDS
    pix
    
DESCRIPTION

    Inlet for a list - "vert_size"
    Inlet for a list - "vert_pos"

    "snap" - Snap a pix
    "vert_size" - Set the size of the pix
    "vert_pos" - Set the position of the pix
    
-----------------------------------------------------------------*/
class GEM_EXTERN fux_glitch : public GemBase
{
    CPPEXTERN_HEADER(fux_glitch, GemBase)

    public:

        //////////
        // Constructor
    	fux_glitch(int argc, t_atom *argv);
    	
    protected:
    	
    	//////////
    	// Destructor
    	virtual ~fux_glitch();

    	//////////
    	// When a snap is received
    	virtual void	snapMess();

    	//////////
    	// When a snap is received
    	virtual void	glitchAmount(int gsize);

    	//////////
    	// Do the rendering
    	virtual void 	render(GemState *state);

    	//////////
    	// Clear the dirty flag on the pixBlock
    	virtual void 	postrender(GemState *state);

    	//////////
    	// When a size message is received
    	virtual void	sizeMess(int width, int height);
    	
    	//////////
    	// When a position message is received
virtual void	posMess(int x, int y);
    	
    	//////////
    	// Clean up the image
    	void	    	cleanImage();
    	
    	//////////
    	// The original fux_glitch
    	imageStruct 	*m_originalImage;
    	
    	//////////
    	// The pixBlock with the current image
    	pixBlock    	m_pixBlock;
		imageStruct     m_imageStruct;
    	
    	//////////
    	// The x position
    	int     	m_x;
    	
    	//////////
    	// The y position
    	int     	m_y;
    	
    	//////////
    	// The width
    	int     	m_width;
    	
    	//////////
    	// The height
    	int     	m_height;
    	
		//////////
		// Glitch Data
		int GLITCH_AMOUNT;	
		
		int		m_blur;
		float	m_glitchAmount;
		float	m_glitchLength;
		
		
		t_inlet*	inletLength;
    

    private:
    	
    	//////////
    	// static member functions
    	static void 	snapMessCallback(void *data);
    	static void 	sizeMessCallback(void *data, t_floatarg width, t_floatarg height );
    	static void 	posMessCallback(void *data, t_floatarg x, t_floatarg y);
		static void     glitchAmountCallback(void *data, t_floatarg size);
		static void     lengthCallback(void *data, t_floatarg size);
		
	
};

#endif	// for header file
