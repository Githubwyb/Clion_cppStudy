#ifndef SMART_ENSURE_H__
#define SMART_ENSURE_H__
#include "smart_assert.h"

//是否启用boost stacktrace打堆栈
#ifdef USE_BOOST_STACKTRACE
#define BOOST_STACKTRACE_GNU_SOURCE_NOT_REQUIRED
#include <boost/stacktrace.hpp>
#endif

#define ENSURE_BASE(expr) \
    static_assert(std::is_same<decltype(expr), bool>::value, "ENSURE(expr) can only be used on bool expression"); \
    if ( (expr) ) ; \
    else  SMART_ENSURE(expr) \

#ifdef USE_BOOST_STACKTRACE
#define ENSURE_TRACE(expr) \
    static_assert(std::is_same<decltype(expr), bool>::value, "ENSURE(expr) can only be used on bool expression"); \
    if ( (expr) ) ; \
    else  std::cout << boost::stacktrace::stacktrace() << std::endl; SMART_ENSURE(expr) \


#define ENSURE_DUMP(expr) \
    static_assert(std::is_same<decltype(expr), bool>::value, "ENSURE(expr) can only be used on bool expression"); \
    if ( (expr) ) ; \
    else  boost::stacktrace::safe_dump_to("./crash.dmp");  SMART_ENSURE(expr) \

#else
#define ENSURE_TRACE ENSURE_BASE
#pragma message("You do not install boost_stacktrace,ENSURE_TRACE/ENSURE_DUMP  dont work.but  ENSURE work")
#define ENSURE_DUMP ENSURE_BASE
#endif


#define ENSURE(expr) ENSURE_TRACE(expr)

#if defined(DEBUG) or defined(_DEBUG) or defined(DBG) or defined(_DBG) or defined(DEBUG_GLOBAL) or defined(DEBUG_) or defined(DBG_)
#define ENSURE_DEBUG(expr) ENSURE(expr)
#else
#define ENSURE_DEBUG(expr) ENSURE(true)
#endif

#endif
