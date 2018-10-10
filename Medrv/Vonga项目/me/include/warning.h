/**************************************************************************\
 *
 *                      Hopen Window System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd.
 *                       All Rights Reserved
 *
 *
 * Filename     warning.h
 *
 * Purpose      ���¶��� Microsoft Visual C++��warning����ѡ��, ʹVC�ı���
 *              Ч��������ƽ̨������һ��. ʹ�ñ�ͷ�ļ�ʱ�뽫VC���������е�
 *              ���漶������Ϊ4��, �������о���.
 *  
 * Author       WeiZhong
 *
\**************************************************************************/

#ifndef __WARNING_H
#define __WARNING_H

#ifdef _MSC_VER /* For Microsoft Visual C++ */

#pragma warning(disable : 4001) /* nonstandard extension 'single line     */
                                /* comment' was used                      */
#pragma warning(disable : 4100) /* unreferenced formal parameter          */

/* Disable the unimportant warnings if WARNING_ALL is undefined */
#ifndef WARNING_ALL

#pragma warning(disable : 4127) /* conditional expression is constant     */
#pragma warning(disable : 4244) /* conversion from int to unsigned short  */
#pragma warning(disable : 4310) /* cast truncates constant value          */
#pragma warning(disable : 4505) /* unreferenced local function has been   */
                                /* removed                                */
#pragma warning(disable : 4701) /* local variable may be used without     */
                                /* having been initialized                */

#endif /* WARNING_ALL */

/* Report the specified warnings as errors */
#pragma warning(error : 4057)   /* 'identifier1' indirection to slightly  */
                                /* different base types from 'identifier2'*/

#pragma warning(error : 4013)   /* 'function' undefined; assuming extern  */
                                /* returning int                          */
#pragma warning(error : 4028)   /* formal parameter 'number' different    */
                                /* from declaration                       */
#pragma warning(error : 4029)   /* declared formal parameter list         */
                                /* different from definition              */
#pragma warning(error : 4033)   /* function' must return a value          */


#pragma warning(error : 4142)   /* benign redefinition of type            */
                                /* A type was redefined, but the          */
                                /* redefinition had no effect on the code */
                                /* generated.                             */

#endif /* _MSC_VER */

#endif  /* __WARNING_H */
