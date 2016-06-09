#include <admesh/stl.h>
#include <cstdio>
#include "tclap/CmdLine.h"

#define outputfile "out.pov"

stl_file stl;
float scalefactor=1.0;

int loadandfixstl(std::string input){
    stl_open(&stl, const_cast<char*>(input.c_str()));
    if (stl.stats.number_of_facets == 0) return 1;
    
    // checking exact
    stl_check_facets_exact(&stl);
    stl.stats.facets_w_1_bad_edge = (stl.stats.connected_facets_2_edge - stl.stats.connected_facets_3_edge);
    stl.stats.facets_w_2_bad_edge = (stl.stats.connected_facets_1_edge - stl.stats.connected_facets_2_edge);
    stl.stats.facets_w_3_bad_edge = (stl.stats.number_of_facets - stl.stats.connected_facets_1_edge);
    
    // checking nearby
    float tolerance = stl.stats.shortest_edge;
    float increment = stl.stats.bounding_diameter / 10000.0;
    int iterations = 2;
    if (stl.stats.connected_facets_3_edge < stl.stats.number_of_facets) {
        for (int i = 0; i < iterations; i++) {
            if (stl.stats.connected_facets_3_edge < stl.stats.number_of_facets) {
                stl_check_facets_nearby(&stl, tolerance);
                tolerance += increment;
            } else {
                break;
            }
        }
    }
    
    // remove_unconnected
    if (stl.stats.connected_facets_3_edge <  stl.stats.number_of_facets) {
        stl_remove_unconnected_facets(&stl);
    }
    
    // fill_holes
    if (stl.stats.connected_facets_3_edge < stl.stats.number_of_facets) {
        stl_fill_holes(&stl);
        stl_clear_error(&stl);
    }
    
    // normal_directions
    stl_fix_normal_directions(&stl);
    
    // normal_values
    stl_fix_normal_values(&stl);
    
    // always calculate the volume and reverse all normals if volume is negative
    stl_calculate_volume(&stl);
    
    // neighbors
    stl_verify_neighbors(&stl);
    float maxdim=std::max(stl.stats.max.x-stl.stats.min.x, std::max(stl.stats.max.y-stl.stats.min.y,stl.stats.max.z-stl.stats.min.z));
    scalefactor=10.0/maxdim;
    stl_scale(&stl, scalefactor);
    stl_rotate_x(&stl, -90);
    stl_translate_relative(&stl, -stl.stats.min.x, -stl.stats.min.y, -stl.stats.min.z);
    stl_translate_relative(&stl, (stl.stats.min.x-stl.stats.max.x)/2, (stl.stats.min.y-stl.stats.max.y)/2, (stl.stats.min.z-stl.stats.max.z)/2);
    stl_translate_relative(&stl, 0, -(stl.stats.min.y), 0);
    stl_invalidate_shared_vertices(&stl);
    //printf("X %f - %f ; Y %f - %f; Z %f - %f.\n",stl.stats.min.x,stl.stats.max.x,stl.stats.min.y,stl.stats.max.y,stl.stats.min.z,stl.stats.max.z);
    return 0;
}

int main(int argc, char **argv){
    
    try{
        TCLAP::CmdLine cmd("Generates geometry for povray from an STL input. Scales all objects such that the maximum dimension is ten units, and the objects are centered in X and Z and have positive Y, touching the Y=0 plane. The objects are rotated by 90 degrees around X.", ' ', "0.0.1");
        TCLAP::ValueArg<std::string> outputArg("o","output","File to output results to, defaults to out.pov, must match include in pov file",false,outputfile,"output file name");
        cmd.add( outputArg );
        TCLAP::UnlabeledValueArg<std::string> input("inputfile","Input STL file name", true, "", "input file name");
        cmd.add(input);
        cmd.parse(argc,argv);
        if(loadandfixstl(input.getValue())){
            printf("Input file could not be opened or contains no geometry.\n");
            return 1;
        }
    
        FILE* f = fopen(outputArg.getValue().c_str(), "w");
        //fprintf(f,"mesh\n{\n");
        for (int i = 0; i < stl.stats.number_of_facets; i++) {
            stl_facet* facet = &stl.facet_start[i];
            fprintf(f,"triangle { ");
            for(int j=0;j<3;j++){
                fprintf(f,"<%f,%f,%f>%s",facet->vertex[j].x,facet->vertex[j].y,facet->vertex[j].z,(j<2)?", ":"");
            }
            fprintf(f,"} \n");
        }
        return 0;
        
    
        
    }catch (TCLAP::ArgException &e) 
	{ std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }
    
    
    
}
