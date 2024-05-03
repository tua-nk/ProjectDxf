/*!
 * \file section.c
 *
 * \author Copyright (C) 2008, 2009, 2010, 2011, 2012, 2014, 2015, 2017
 * by Bert Timmerman <bert.timmerman@xs4all.nl>.
 *
 * \author Copyright (C) 2010 by Luis Matos <gass@otiliamatos.ath.cx>.
 *
 * \author Copyright (C) 2018 Calvin Tower <Calvin@CorianderDesigns.com>
 *
 * \brief Functions for a DXF section entity (\c SECTION).
 *
 * <hr>
 * <h1><b>Copyright Notices.</b></h1>\n
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.\n\n
 * This program is distributed in the hope that it will be useful, but
 * <b>WITHOUT ANY WARRANTY</b>; without even the implied warranty of
 * <b>MERCHANTABILITY</b> or <b>FITNESS FOR A PARTICULAR PURPOSE</b>.\n
 * See the GNU General Public License for more details.\n\n
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to:\n
 * Free Software Foundation, Inc.,\n
 * 59 Temple Place,\n
 * Suite 330,\n
 * Boston,\n
 * MA 02111 USA.\n
 * \n
 * Drawing eXchange Format (DXF) is a defacto industry standard for the
 * exchange of drawing files between various Computer Aided Drafting
 * programs.\n
 * DXF is an industry standard designed by Autodesk(TM).\n
 * For more details see http://www.autodesk.com.
 * <hr>
 */


#include "section.h"


/*!
 * \brief Function reads a SECTION in a DXF file.
 */
int
dxf_section_read
(
        DxfFile *fp
                /*!< DXF file handle of input file (or device). */
)
{
#if DEBUG
        DXF_DEBUG_BEGIN
#endif
        char temp_string[DXF_MAX_STRING_LENGTH];
        DxfHeader dxf_header;
        DxfBlock dxf_block;
        char *dxf_entities_list = NULL;

        /* Do some basic checks. */
        if (fp == NULL)
        {
                fprintf (stderr,
                  (_("Error in %s () a NULL file pointer was passed.\n")),
                  __FUNCTION__);
                return (EXIT_FAILURE);
        }
        memset(temp_string, 0, sizeof(temp_string));
        dxf_read_line (temp_string, fp);
        if (strcmp (temp_string, "2") == 0)
        {
                while (!feof (fp->fp)) /* Does this actually work? */
                {
                        memset(temp_string, 0, sizeof(temp_string));
                        dxf_read_line (temp_string, fp);
                        if (strcmp (temp_string, "HEADER") == 0)
                        {
                                /* We have found the begin of the HEADER section. */
                                dxf_header_read (fp, &dxf_header);
                        }
                        else if (strcmp (temp_string, "CLASSES") == 0)
                        {
                                /* We have found the begin of the CLASSES sction. */
                                /*! \todo Invoke a function for parsing the \c CLASSES section. */ 
                        }
                        else if (strcmp (temp_string, "TABLES") == 0)
                        {
                                /* We have found the begin of the TABLES sction. */
                                /*! \todo Invoke a function for parsing the \c TABLES section. */ 
                        }
                        else if (strcmp (temp_string, "BLOCKS") == 0)
                        {
                                /* We have found the begin of the BLOCKS sction. */

                                /*! \todo Experimental usage of block_read */
//                                dxf_read_blocks
//                                (
//                                        fp->fp,
//                                        &dxf_blocks_list,
//                                        dxf_header._AcadVer
//                                );
                        }
                        else if (strcmp (temp_string, "ENTITIES") == 0)
                        {
                                /* We have found the begin of the ENTITIES sction. */
                                dxf_entities_read_table (fp->filename,
                                                   fp->fp,
                                                   fp->line_number,
                                                   dxf_entities_list,
                                                   dxf_header._AcadVer);                        }
                        else if (strcmp (temp_string, "OBJECTS") == 0)
                        {
                                /* We have found the begin of the OBJECTS sction. */
                                /*! \todo Invoke a function for parsing the \c OBJECTS section. */ 
                        }
                        else if (strcmp (temp_string, "THUMBNAIL") == 0)
                        {
                                /* We have found the begin of the THUMBNAIL sction. */
                                /*! \todo Invoke a function for parsing the \c THUMBNAIL section. */ 
                        }
                }
        }
        else
        {
                fprintf (stderr,
                  (_("Warning in %s () unexpected string encountered while reading line %d from: %s.\n")),
                  __FUNCTION__, fp->line_number, fp->filename);
        }
#if DEBUG
        DXF_DEBUG_END
#endif
        return EXIT_SUCCESS;
}


/*!
 * \brief Write DXF output to a file for a section marker.
 */
int
dxf_section_write
(
        DxfFile *fp,
        char *section_name
)
{
#if DEBUG
        DXF_DEBUG_BEGIN
#endif
        /* Do some basic checks. */
        if (fp == NULL)
        {
                fprintf (stderr,
                  (_("Error in %s () a NULL file pointer was passed.\n")),
                  __FUNCTION__);
                return (EXIT_FAILURE);
        }
        if (section_name == NULL)
        {
                fprintf (stderr,
                  (_("Error in %s () a NULL pointer was passed.\n")),
                  __FUNCTION__);
                return (EXIT_FAILURE);
        }
        fprintf (fp->fp, "  0\nSECTION\n  2\n%s\n", section_name);
#if DEBUG
        DXF_DEBUG_END
#endif
        return (EXIT_SUCCESS);
}


/* EOF */
