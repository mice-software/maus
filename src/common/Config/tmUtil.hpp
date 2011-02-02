/**
 * @author      Takashi Matsushita
 * @date        $Date: 2006-10-18 13:25:06 $
 * @version     $Revision: 1.2 $
 */
/*
 * Copyright:   (c) 2005 Takashi Matsushita
 * Licence:     All rights reserved.
 * Created:     2005-02-14
 * Credits:
 */

/** @todo */
/** @warnings */

#ifndef tmUtil_hpp
#define tmUtil_hpp
/*====================================================================*
 * declarations
 *====================================================================*/
#include <iostream>

/** @file tmUtil.hpp
    correction of macros */

/** a stringize macro; private use */
#define TM_STR_(x) #x

/** a stringize macro; public use */
#define TM_STR(x) TM_STR_(x)

/** a macro that displays message then exits a program */
#define TM_FATAL_ERROR(x) (std::cout << "fat> " << x << " [" \
                                     << __FILE__ << ":" << __LINE__ << "]" \
                                     << std::endl, \
                           exit(EXIT_FAILURE))

/** a macro to be used for incomplete implementation */
#define TM_NOT_IMPLEMENTED_ERROR() TM_FATAL_ERROR("not implemented")

/** a macro that displays run-time information message */
#define TM_LOG_INF(x) ( std::cout << "inf> " << x << std::endl )

/** a macro that displays run-time warning message */
#define TM_LOG_WAR(x) ( std::cout << "war> " << x << std::endl )

/** a macro that displays run-time error message */
#define TM_LOG_ERR(x) ( std::cout << "err> " << x << std::endl )


/** a macro that displays run-time debug message,
    can be disabled by defining NDEBUG macro */
#ifndef NDEBUG
#define TM_LOG_DBG(x) (std::cout << "dbg> " << x  << " (" << __FILE__ \
                                 << ":" << __LINE__ << ")" << std::endl)
#else
#define TM_LOG_DBG(x) ;
#endif

#endif // tmUtil_hpp
/* eof */
