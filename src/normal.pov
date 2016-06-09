#version 3.7;
global_settings{assumed_gamma 1.0}
default{ finish{ ambient 0.1 diffuse 0.9 }}
//------------------------------------------
#include "colors.inc"
#include "textures.inc"
//------------------------------------------
// camera ----------------------------------
#if(clock=6)
camera{ orthographic 
        location  <0.0 , 10.0 ,0.0>
        look_at   <0.0 , -1.0 , 0.0>
        right x*image_width/image_height
        angle 75
        }
#end
#if(clock=5)
camera{ 
        location  <0.0 , 12.0 ,-12.0>
        look_at   <0.0 , 0.0 , 0.0>
        right x*image_width/image_height
        angle 75 
        }

#end
#if(clock<5)
camera{ 
        location  <0.0 , 1.0 ,-12.0>
        look_at   <0.0 , 1.0 , 0.0>
        right x*image_width/image_height
        angle 75 
        }

#end

// sun -------------------------------------
#for(Angle, 0, 360, 120)
light_source{<1500,1000,-2500> color White rotate<0,Angle,0>}
#end
// ground ----------------------------------
plane{ <0,1,0>, 0
       texture{
          pigment{ color rgb<0.1,0.1,0.1>}
          //normal { bumps 0.75 scale 0.15 }
          #if(clock<6)finish { reflection {0.8} } #end
       } // end of texture
       translate<0,-1,0>
     } // end of plane

#declare Stripes=function(v) {(mod((200*v),10)/20)}

mesh{
    
#include "out.pov"
texture{
          pigment{ color rgb<0.1,0.1,0.9>}
          normal { function{Stripes(y)} 0.1 rotate<0,0,0>}
          finish { phong 0.1 }
          
       } // end of texture
       #if(clock<5)
       rotate <0,clock*90,0>
       #end
       #if(clock=5)
       rotate <0,45,0>
       #end
       translate<0,-.99,0>
       
}

//------------------------------------- end 


