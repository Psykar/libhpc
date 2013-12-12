#include "text.hh"

#ifdef HAVE_GLUT

namespace hpc {
   namespace gl {

      void
      draw_text( const string& text,
                 GLfloat x,
                 GLfloat y,
                 void* font )
      {
         glRasterPos2f( x, y );
         for( char c : text )
            glutBitmapCharacter( font, c );
      }

   }
}

#endif