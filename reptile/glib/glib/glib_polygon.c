 /*************************************************************************//**
 * @file glib_polygon.c
 * @brief Silicon Labs Graphics Library: Polygon Drawing Routines
 ******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/


/* C-header files */
#include <stdint.h>

/* EM types */
#include "em_types.h"

/* GLIB header files */
#include "glib.h"

/**************************************************************************//**
*  @brief
*  Draws a polygon using Bresnham's Midpoint Line Algorithm.
*
*  This function draws a line between all points outlining the polygon.
*  The first and last point doesn't have to be the same. The function automatically
*  draws a line from the start point to the end point.
*
*  @param pContext
*  Pointer to a GLIB_Context_t which the line is drawn. The lines are drawn using the
*  foreground color.
*  @param numPoints
*  Number of points in the polygon ( Has to be greater than 1 )
*  @param polyPoints
*  Pointer to array of polygon points. The points are laid out like this: polyPoints = {x1,y1,x2,y2 ... }
*  Polypoints has to contain at least (numPoints * 2) entries
*
*  @return
*  Returns GLIB_OK on if at least one element was drawn, or else error code
******************************************************************************/

EMSTATUS GLIB_drawPolygon(GLIB_Context_t *pContext,
                          uint32_t numPoints, int32_t *polyPoints)
{
  EMSTATUS status;
  uint32_t drawnElements = 0;
  uint32_t point;
  int32_t firstX;
  int32_t firstY;
  int32_t startX;
  int32_t startY;
  int32_t endX;
  int32_t endY;

  /* Check arguments */
  if (pContext == NULL || polyPoints == NULL || numPoints < 2) return GLIB_ERROR_INVALID_ARGUMENT;

  startX = *polyPoints++;
  startY = *polyPoints++;
  firstX = startX;
  firstY = startY;

  /* Loop through the points by moving the pointer */
  for (point = 1; point < numPoints; point++)
  {
    endX = *polyPoints++;
    endY = *polyPoints++;

    /* Draw a line between each pair of points */
    status = GLIB_drawLine(pContext, startX, startY, endX, endY);
    if (status > GLIB_ERROR_NOTHING_TO_DRAW) return status;
    if (status == GLIB_OK) drawnElements++;

    startX = endX;
    startY = endY;
  }

  /* Draw a line from last point to first point */
  if ((endX != firstX) || (endY != firstY))
  {
    status = GLIB_drawLine(pContext, firstX, firstY, endX, endY);
    if (status > GLIB_ERROR_NOTHING_TO_DRAW) return status;
    if (status == GLIB_OK) drawnElements++;
  }
  return ((drawnElements == 0) ? GLIB_ERROR_NOTHING_TO_DRAW : GLIB_OK);
}
