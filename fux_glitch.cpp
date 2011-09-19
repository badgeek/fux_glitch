////////////////////////////////////////////////////////
//
// GEM - Graphics Environment for Multimedia
//
// zmoelnig@iem.kug.ac.at
//
// Implementation file
//
//    Copyright (c) 1997-1999 Mark Danks.
//    Copyright (c) Günther Geiger.
//    Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::für::umläute. IEM
//    For information on usage and redistribution, and for a DISCLAIMER OF ALL
//    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.
//
/////////////////////////////////////////////////////////

#include "fux_glitch.h"

#include "Base/GemMan.h"
#include "Base/GemCache.h"

CPPEXTERN_NEW_WITH_GIMME(fux_glitch)

/////////////////////////////////////////////////////////
//
// fux_glitch
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
fux_glitch :: fux_glitch(int argc, t_atom *argv)
    	  : m_originalImage(NULL)
{
  m_pixBlock.image = m_imageStruct;
  m_pixBlock.image.data = NULL;
  if (argc == 4){
    m_x = (int)atom_getfloat(&argv[0]);
    m_y = (int)atom_getfloat(&argv[1]);
    m_width = (int)atom_getfloat(&argv[2]);
    m_height = (int)atom_getfloat(&argv[3]);
  } else if (argc == 2)	{
    m_x = m_y = 0;
    m_width = (int)atom_getfloat(&argv[0]);
    m_height = (int)atom_getfloat(&argv[1]);
  } else if (argc == 0)	{
    m_x = m_y = 0;
    m_width = m_height = 128;
  } else {
    error("needs 0, 2, or 4 values");
    m_x = m_y = 0;
    m_width = m_height = 128;
  }
  inlet_new(this->x_obj, &this->x_obj->ob_pd, gensym("list"), gensym("vert_pos"));
  inlet_new(this->x_obj, &this->x_obj->ob_pd, gensym("list"), gensym("vert_size"));
  inlet_new(this->x_obj, &this->x_obj->ob_pd, gensym("float"), gensym("glitch_amount"));

  GLITCH_AMOUNT = 20;
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
fux_glitch :: ~fux_glitch()
{
    cleanImage();
}

/////////////////////////////////////////////////////////
// snapMess
//
/////////////////////////////////////////////////////////
void fux_glitch :: snapMess()
{


  if(!GLEW_VERSION_1_1 && !GLEW_EXT_texture_object) return;

  if (m_cache&&m_cache->m_magic!=GEMCACHE_MAGIC)
    m_cache=NULL;
        
	if (m_width <= 0 || m_height <= 0)
	{
		error("Illegal size");
		return;
	}
	// do we need to remake the data?
	int makeNew = 0;

    // release previous data
    if (m_originalImage)
    {
		if (m_originalImage->xsize != m_width ||
			m_originalImage->ysize != m_height)
		{
			m_originalImage->clear();
			delete m_originalImage;
			m_originalImage = NULL;
			makeNew = 1;
		}
	}
	else
		makeNew = 1;

    if (makeNew)
	{
		m_originalImage = new imageStruct;
		m_originalImage->xsize = m_width;
		m_originalImage->ysize = m_height;
                /* magic: on __APPLE__ this could actually set to GL_BGRA_EXT ! */
                m_originalImage->setCsizeByFormat(GL_RGBA);
                m_originalImage->upsidedown = 0;

		m_originalImage->allocate(m_originalImage->xsize * m_originalImage->ysize * m_originalImage->csize);
	}

    glFinish();
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glPixelStorei(GL_PACK_ROW_LENGTH, 0);
    glPixelStorei(GL_PACK_SKIP_ROWS, 0);
    glPixelStorei(GL_PACK_SKIP_PIXELS, 0);

    glReadPixels(m_x, m_y, m_width, m_height, GL_BGR, GL_UNSIGNED_BYTE, m_originalImage->data);   
    
    BYTE *GLITCH_DATA; 
    int GLITCH_SIZE;	
    int GLITCH_START = 100;

    // Load image from ImageStruct to FreeImage Memory
    FIBITMAP *GLITCH_FUCK = FreeImage_ConvertFromRawBits((BYTE*)m_originalImage->data, m_width, m_height, 3 * m_width, 24, 0, 0, 0, false);
    FIMEMORY *GLITCH_MEM  = FreeImage_OpenMemory();
    //FIMEMORY *GLITCH_MEM_FINAL  = FreeImage_OpenMemory();
    
    // Resave image as JPG to Memory
    FreeImage_SaveToMemory(FIF_JPEG, GLITCH_FUCK, GLITCH_MEM, 0);

    // Get Access to memory
    FreeImage_AcquireMemory(GLITCH_MEM, &GLITCH_DATA, (DWORD*) &GLITCH_SIZE);

    // Fuck the memory
    for(int i = 0; i < GLITCH_AMOUNT; i++)
    {
    	int GLITCH_POS = (GLITCH_START + random()) % (int) GLITCH_SIZE;
    	GLITCH_DATA[GLITCH_POS] = '\0'; 
    }
    
    // Rewind to Start of mem or Blank image
    FreeImage_SeekMemory(GLITCH_MEM, 0L, SEEK_SET);
    
    //Reload GLITCHED image
    FIBITMAP *GLITCH_SAVED = FreeImage_LoadFromMemory(FIF_JPEG, GLITCH_MEM, 0);

    int fwidth = FreeImage_GetWidth(GLITCH_SAVED); 
    int fheight = FreeImage_GetHeight(GLITCH_SAVED); 


	if (FreeImage_HasPixels(GLITCH_SAVED)){

    	int bytespp = FreeImage_GetLine(GLITCH_SAVED) / FreeImage_GetWidth(GLITCH_SAVED);
   		unsigned char *pixels = m_originalImage->data;
		
		for(int y = 0; y < fheight; y++) {
			BYTE *bits = FreeImage_GetScanLine(GLITCH_SAVED, y);
			for(int x = 0; x < fwidth; x++) {
				pixels[chRed]   = (unsigned char) bits[FI_RGBA_RED];
				pixels[chGreen] = (unsigned char) bits[FI_RGBA_GREEN];
				pixels[chBlue ] = (unsigned char) bits[FI_RGBA_BLUE];
				pixels += 4;
				bits += bytespp;
			}
		}
	}else{
		post("FUX_GLITCH-> bitmap too fucked to be displayed...");
	}


    FreeImage_CloseMemory(GLITCH_MEM);
    FreeImage_Unload(GLITCH_FUCK);	    
    FreeImage_Unload(GLITCH_SAVED);	    
    free(GLITCH_DATA);
    
    //post("FUX_GLITCH-> gem-width:%i gem-height:%i f-width:%i f-height:%i x:%i y:%i size:%i type:%i", m_width, m_height, fwidth, fheight, m_x, m_y, GLITCH_SIZE, m_originalImage->type);

    if (m_cache)
		m_cache->resendImage = 1;

}

/////////////////////////////////////////////////////////
// render
//
/////////////////////////////////////////////////////////
void fux_glitch :: render(GemState *state)
{
    // if we don't have an image, just return
    if (!m_originalImage)
		return;
    
    // do we need to reload the image?    
    if (m_cache&&m_cache->resendImage)
    {
      m_originalImage->refreshImage(&m_pixBlock.image);
    	m_pixBlock.newimage = 1;
    	m_cache->resendImage = 0;
    }
    
    state->image = &m_pixBlock;

	//post("render state\n");
}

/////////////////////////////////////////////////////////
// postrender
//
/////////////////////////////////////////////////////////
void fux_glitch :: postrender(GemState *state)
{
    m_pixBlock.newimage = 0;
    state->image = NULL;
}

/////////////////////////////////////////////////////////
// sizeMess
//
/////////////////////////////////////////////////////////
void fux_glitch :: sizeMess(int width, int height)
{
    m_width = width;
    m_height = height;
}

/////////////////////////////////////////////////////////
// GLITCH_SIZE
//
/////////////////////////////////////////////////////////
void fux_glitch :: glitchAmount(int gsize)
{
     GLITCH_AMOUNT = gsize;
}

/////////////////////////////////////////////////////////
// posMess
//
/////////////////////////////////////////////////////////
void fux_glitch :: posMess(int x, int y)
{
    m_x = x;
    m_y = y;
}

/////////////////////////////////////////////////////////
// cleanImage
//
/////////////////////////////////////////////////////////
void fux_glitch :: cleanImage()
{
    // release previous data
    if (m_originalImage)
    {
      delete m_originalImage;
      m_originalImage = NULL;

      m_pixBlock.image.clear();
    }
}

/////////////////////////////////////////////////////////
// static member functions
//
/////////////////////////////////////////////////////////
void fux_glitch :: obj_setupCallback(t_class *classPtr)
{
    class_addmethod(classPtr, (t_method)&fux_glitch::snapMessCallback,
    	    gensym("snap"), A_NULL);
    class_addbang(classPtr, (t_method)&fux_glitch::snapMessCallback);
    class_addmethod(classPtr, (t_method)&fux_glitch::sizeMessCallback,
    	    gensym("vert_size"), A_FLOAT, A_FLOAT, A_NULL);
    class_addmethod(classPtr, (t_method)&fux_glitch::posMessCallback,
    	    gensym("vert_pos"), A_FLOAT, A_FLOAT, A_NULL);
    class_addmethod(classPtr, (t_method)&fux_glitch::glitchAmountCallback,
    	    gensym("glitch_amount"), A_FLOAT);

}
void fux_glitch :: snapMessCallback(void *data)
{
    GetMyClass(data)->snapMess();
}

void fux_glitch :: sizeMessCallback(void *data, t_floatarg width, t_floatarg height)
{
    GetMyClass(data)->sizeMess((int)width, (int)height);
}

void fux_glitch :: glitchAmountCallback(void *data, t_floatarg size)
{
    GetMyClass(data)->glitchAmount((int)size);
}


void fux_glitch :: posMessCallback(void *data, t_floatarg x, t_floatarg y)
{
    GetMyClass(data)->posMess((int)x, (int)y);
}
