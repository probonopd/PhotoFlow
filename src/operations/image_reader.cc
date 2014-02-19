/* 
 */

/*

    Copyright (C) 2014 Ferrero Andrea

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.


 */

/*

    These files are distributed with PhotoFlow - http://aferrero2707.github.io/PhotoFlow/

 */

#include "image_reader.hh"
//#include "../vips/vips_layer.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

int
vips_cast( VipsImage *in, VipsImage **out, VipsBandFormat format, ... );

#ifdef __cplusplus
}
#endif /*__cplusplus*/


VipsImage* PF::ImageReaderPar::build(std::vector<VipsImage*>& in, int first, 
				     VipsImage* imap, VipsImage* omap)
{
  //VipsImage* outnew;

  // Create VipsImage from given file; unref previously opened image if any
  //if( image ) g_object_unref( image );
  image = vips_image_new_from_file( file_name.get().c_str() );
  //g_object_ref( image );
  

  std::cout<<"ImageReaderPar::build(): "<<std::endl
	   <<"input images:"<<std::endl;
  for(int i = 0; i < in.size(); i++) {
    std::cout<<"  "<<(void*)in[i]<<std::endl;
  }
  std::cout<<"imap: "<<(void*)imap<<std::endl<<"omap: "<<(void*)omap<<std::endl;



  void *data;
  size_t data_length;
  if( !vips_image_get_blob( image, VIPS_META_ICC_NAME, 
			    &data, &data_length ) ) {
    cmsHPROFILE profile_in = cmsOpenProfileFromMem( data, data_length );
    if( profile_in ) {  
      char tstr[1024];
      cmsGetProfileInfoASCII(profile_in, cmsInfoDescription, "en", "US", tstr, 1024);
      std::cout<<"ImageReader: Embedded profile found: "<<tstr<<std::endl;
      cmsCloseProfile( profile_in );
    }
  }


  VipsImage* out = image;

  //if( get_format() != image->BandFmt ) {
    //vips_call( "cast", image, &out, "format", get_format(), NULL );
  vips_cast( image, &out, get_format(), NULL );
    //VIPS_UNREF( image );
    g_object_unref( image );
    //}
  /*
  if( vips_image_get_typeof(out, VIPS_META_ICC_NAME) )
    return NULL;
  vips_image_set_blob( out, VIPS_META_ICC_NAME, 
		       NULL, data, data_length );
  */
  if( !vips_image_get_blob( out, VIPS_META_ICC_NAME, 
			    &data, &data_length ) ) {
    
    cmsHPROFILE profile_in = cmsOpenProfileFromMem( data, data_length );
    if( profile_in ) {  
      char tstr[1024];
      cmsGetProfileInfoASCII(profile_in, cmsInfoDescription, "en", "US", tstr, 1024);
      std::cout<<"ImageReader: Embedded profile found: "<<tstr<<std::endl;
      cmsCloseProfile( profile_in );
    }
  }
  /**/

  set_image( out );
  std::cout<<"out: "<<(void*)out<<std::endl<<std::endl;
  return out;

  // Prepare the blending step between the new image (in in2[1]) and the underlying image
  // if existing (in in2[0]).
  // The blending code will simply force the mode to "passthrough" and copy in2[1] to outnew
  // if in2[0] is NULL
  std::vector<VipsImage*> in2;
  if( !in.empty() ) in2.push_back( in[0] );
  else in2.push_back( NULL );
  in2.push_back( image );

  blender->get_par()->build( in2, 0, imap, omap);

  set_image( blender->get_par()->get_image() );
}
