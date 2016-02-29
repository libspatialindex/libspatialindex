/******************************************************************************
 * Project:  libspatialindex - A C++ library for spatial indexing
 * Author:   Norman Barker, norman.barker@gmail.com
 ******************************************************************************
 * Copyright (c) 2015, Norman Barker
 *
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <spatialindex/capi/sidx_api.h>

void load(IndexH idx);
void query(IndexH idx);
void bounds(IndexH idx);

int main()
{
   char* pszVersion = SIDX_Version();
   fprintf(stdout, "libspatialindex version: %s\n", pszVersion);
   fflush(stdout);
   free(pszVersion);

   IndexPropertyH props = IndexProperty_Create();

   // create an in-memory r*-tree index
   IndexProperty_SetIndexType(props, RT_RTree);
   IndexProperty_SetIndexStorage(props, RT_Memory);
   IndexH idx = Index_Create(props);
   IndexProperty_Destroy(props); 

   if (Index_IsValid(idx))
   {
       load(idx);
       bounds(idx);
       query(idx);
       Index_Destroy(idx);
   }
   else
   {
       printf("Failed to create valid index\n");
   }
   return 0; 
}

void load(IndexH idx)
{
    double min[] = {0.5, 0.5};
    double max[] = {0.5, 0.5};
    Index_InsertData(idx, 1, min, max, 2, 0, 0);
}

void query(IndexH idx)
{
    double min[] = {0.0, 0.0};
    double max[] = {1.0, 1.0};
    uint64_t nResults;
    Index_Intersects_count(idx, min, max, 2, &nResults);
    if (nResults == 1)
         printf("Successful Query\n");
    else
         printf("Failed to execute query\n");
}

void bounds(IndexH idx)
{
    uint32_t dims;
    double* pMins;
    double* pMaxs;
    Index_GetBounds(idx, &pMins, &pMaxs, &dims);
    free(pMins);
    free(pMaxs);

    if (dims == 2)
        printf("Successful bounds query\n");
    else
        printf("Failed to execute bounds query\n");
}
