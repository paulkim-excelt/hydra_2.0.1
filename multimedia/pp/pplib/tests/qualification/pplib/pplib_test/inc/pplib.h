/*++++++++++++++++++++++++++++++ FileHeaderBegin +++++++++++++++++++++++++++++++
 * (c) videantis GmbH
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * FILENAME:             $RCSfile: $
 *
 *------------------------------------------------------------------------------
 *
 * RESPONSIBLE:
 *
 * LAST CHECKED IN BY:  $Author: $, $Date: $
 *
 * NOTES:
 *
 * MODIFIED:
 *
 * $Log: $
 *
 *++++++++++++++++++++++++++++++ FileHeaderEnd +++++++++++++++++++++++++++++++*/



#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <limits.h>

#define DIV_ROUND_INT(n,d) ((((n) < 0) ^ ((d) < 0)) ? (((n) - (d)/2)/(d)) : (((n) + (d)/2)/(d)))
#define DIV_ROUND_UINT(n,d) ((((n) + (d)/2)/(d)))

#define uchar unsigned char
#define cv_to_be(val) ((((val)&0x000000FF)<<24)|(((val)&0x0000FF00)<<8)|(((val)&0x00FF0000)>>8)|((val)&0xFF000000)>>24)
#define DEB_D printf

#define msgIdcvTutorial 0xff42

#define NUM_MPCORES 1


typedef struct CvMat
{
    int type;
    int step;

    union
    {
        uchar* ptr;
        short* s;
        int* i;
        float* fl;
        double* db;
    } data;

    union
    {
        int rows;
        int height;
    };

    union
    {
        int cols;
        int width;
    };


}
CvMat;




static void exec_algorithm(CvMat* src, CvMat* dst,CvMat* mat, CvMat* buf);
static void print_help(int exval);
static void parse_arguments(int argc, char** argv);
