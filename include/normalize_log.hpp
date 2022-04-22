//
// 编译时格式化 Log 的五要素
// Created by hsz on 18-10-29.
//
#ifndef CLION_CPPSTUDY_NORMALIZE_LOG_H
#define CLION_CPPSTUDY_NORMALIZE_LOG_H

#ifndef NORMALIZE_LOG_API
#define NORMALIZE_LOG_API
#endif

#ifndef NORMALIZE_LOG_WITHCTX_API
#define NORMALIZE_LOG_WITHCTX_API
#endif

/**
 * @brief 日志级别
 *
 */
enum normalize_log_field_t {
    LOG_FIELD_WHAT,
    LOG_FIELD_REASON,
    LOG_FIELD_WILL,
    LOG_FIELD_HOW_TO,
    LOG_FIELD_CAUSED_BY,
};

// 5要素工具宏定义
#define WHAT(fmt, ...) (LOG_FIELD_WHAT, fmt, ##__VA_ARGS__)
#define REASON(fmt, ...) (LOG_FIELD_REASON, "; Reason: " fmt, ##__VA_ARGS__)
#define WILL(fmt, ...) (LOG_FIELD_WILL, "; Will: " fmt, ##__VA_ARGS__)
#define HOW_TO(fmt, ...) (LOG_FIELD_HOW_TO, "; HowTo: " fmt, ##__VA_ARGS__)
#define CAUSED_BY(fmt, ...) \
    (LOG_FIELD_CAUSED_BY, "; CausedBy: " fmt, ##__VA_ARGS__)
#define NO_REASON (LOG_FIELD_REASON, "{}", "")
#define NO_WILL (LOG_FIELD_WILL, "{}", "")
#define NO_HOW_TO (LOG_FIELD_HOW_TO, "{}", "")
#define NO_CAUSED_BY (LOG_FIELD_CAUSED_BY, "{}", "")

// 简单的静态断言
#define __LOG_STATIC_ASSERT(e) static_assert(e)

#define __LOG_GET_FIELD(field, ...) field
#define __LOG_GET_FMT(field, fmt, ...) fmt
// ##__VA_ARGS__ 为空时会自动清除前面的逗号
#define __LOG_GET_ARGS(field, fmt, ...) , ##__VA_ARGS__

/**
 * @brief 5要素宏定义
 *
 */
#define NORMALIZE_LOG_5(level, what, reason, will, how_to, caused_by)       \
    do {                                                                    \
        /* 确保参数的顺序正确 */                                   \
        __LOG_STATIC_ASSERT((__LOG_GET_FIELD what) == LOG_FIELD_WHAT);      \
        __LOG_STATIC_ASSERT((__LOG_GET_FIELD reason) == LOG_FIELD_REASON);  \
        __LOG_STATIC_ASSERT((__LOG_GET_FIELD will) == LOG_FIELD_WILL);      \
        __LOG_STATIC_ASSERT((__LOG_GET_FIELD how_to) == LOG_FIELD_HOW_TO);  \
        __LOG_STATIC_ASSERT((__LOG_GET_FIELD caused_by) ==                  \
                            LOG_FIELD_CAUSED_BY);                           \
        /* 确保以下字段不能太短 */                                \
        __LOG_STATIC_ASSERT(sizeof(__LOG_GET_FMT what) > 1);                \
        /* 调用真实的日志函数 */                                   \
        NORMALIZE_LOG_API(                                                  \
            level,                                                          \
            __LOG_GET_FMT what __LOG_GET_FMT reason __LOG_GET_FMT will      \
                __LOG_GET_FMT how_to __LOG_GET_FMT caused_by __LOG_GET_ARGS \
                    what __LOG_GET_ARGS reason __LOG_GET_ARGS will          \
                        __LOG_GET_ARGS how_to __LOG_GET_ARGS caused_by);    \
    } while (0)

/**
 * @brief 3要素宏定义
 *
 */
#define NORMALIZE_LOG_3(level, what, reason, will)                            \
    do {                                                                      \
        /* 确保参数的顺序正确 */                                     \
        __LOG_STATIC_ASSERT((__LOG_GET_FIELD what) == LOG_FIELD_WHAT);        \
        __LOG_STATIC_ASSERT((__LOG_GET_FIELD reason) == LOG_FIELD_REASON);    \
        __LOG_STATIC_ASSERT((__LOG_GET_FIELD will) == LOG_FIELD_WILL);        \
        /* 确保以下字段不能太短 */                                  \
        __LOG_STATIC_ASSERT(sizeof(__LOG_GET_FMT what) > 1);                  \
        /* 调用真实的日志函数 */                                     \
        NORMALIZE_LOG_API(level,                                              \
                          __LOG_GET_FMT what __LOG_GET_FMT reason             \
                              __LOG_GET_FMT will __LOG_GET_ARGS what          \
                                  __LOG_GET_ARGS reason __LOG_GET_ARGS will); \
    } while (0)

/**
 * @brief 2要素宏定义
 *
 */
#define NORMALIZE_LOG_2(level, what, reason)                               \
    do {                                                                   \
        /* 确保参数的顺序正确 */                                  \
        __LOG_STATIC_ASSERT((__LOG_GET_FIELD what) == LOG_FIELD_WHAT);     \
        __LOG_STATIC_ASSERT((__LOG_GET_FIELD reason) == LOG_FIELD_REASON); \
        /* 确保以下字段不能太短 */                               \
        __LOG_STATIC_ASSERT(sizeof(__LOG_GET_FMT what) > 1);               \
        /* 调用真实的日志函数 */                                  \
        NORMALIZE_LOG_API(level,                                           \
                          __LOG_GET_FMT what __LOG_GET_FMT reason          \
                              __LOG_GET_ARGS what __LOG_GET_ARGS reason);  \
    } while (0)

/**
 * @brief 1要素宏定义
 *
 */
#define NORMALIZE_LOG_1(level, what)                                      \
    do {                                                                  \
        /* 确保参数的顺序正确 */                                 \
        __LOG_STATIC_ASSERT((__LOG_GET_FIELD what) == LOG_FIELD_WHAT);    \
        /* 确保以下字段不能太短 */                              \
        __LOG_STATIC_ASSERT(sizeof(__LOG_GET_FMT what) > 1);              \
        /* 调用真实的日志函数 */                                 \
        NORMALIZE_LOG_API(level, __LOG_GET_FMT what __LOG_GET_ARGS what); \
    } while (0)

#ifdef SDPLOG_MORE
#define NORMALIZE_LOG_MORE_1(func, line, level, what)                  \
    do {                                                               \
        /* 确保参数的顺序正确 */                              \
        __LOG_STATIC_ASSERT((__LOG_GET_FIELD what) == LOG_FIELD_WHAT); \
        /* 确保以下字段不能太短 */                           \
        __LOG_STATIC_ASSERT(sizeof(__LOG_GET_FMT what) > 1);           \
        /* 调用真实的日志函数 */                              \
        SDPLOG_MORE(func, line, level,                                 \
                    __LOG_GET_FMT what __LOG_GET_ARGS what);           \
    } while (0)

#define NORMALIZE_LOG_MORE_2(func, line, level, what, reason)              \
    do {                                                                   \
        /* 确保参数的顺序正确 */                                  \
        __LOG_STATIC_ASSERT((__LOG_GET_FIELD what) == LOG_FIELD_WHAT);     \
        __LOG_STATIC_ASSERT((__LOG_GET_FIELD reason) == LOG_FIELD_REASON); \
        /* 确保以下字段不能太短 */                               \
        __LOG_STATIC_ASSERT(sizeof(__LOG_GET_FMT what) > 1);               \
        /* 调用真实的日志函数 */                                  \
        SDPLOG_MORE(func, line, level,                                     \
                    __LOG_GET_FMT what __LOG_GET_FMT reason __LOG_GET_ARGS \
                        what __LOG_GET_ARGS reason);                       \
    } while (0)

#define NORMALIZE_LOG_MORE_3(func, line, level, what, reason, will)            \
    do {                                                                       \
        /* 确保参数的顺序正确 */                                      \
        __LOG_STATIC_ASSERT((__LOG_GET_FIELD what) == LOG_FIELD_WHAT);         \
        __LOG_STATIC_ASSERT((__LOG_GET_FIELD reason) == LOG_FIELD_REASON);     \
        __LOG_STATIC_ASSERT((__LOG_GET_FIELD will) == LOG_FIELD_WILL);         \
        /* 确保以下字段不能太短 */                                   \
        __LOG_STATIC_ASSERT(sizeof(__LOG_GET_FMT what) > 1);                   \
        /* 调用真实的日志函数 */                                      \
        SDPLOG_MORE(func, line, level,                                         \
                    __LOG_GET_FMT what __LOG_GET_FMT reason __LOG_GET_FMT will \
                        __LOG_GET_ARGS what __LOG_GET_ARGS reason              \
                            __LOG_GET_ARGS will);                              \
    } while (0)

#endif  // SDPLOG_MORE

/**
 * @brief 3要素宏定义(支持日志上下文)
 */
#define NORMALIZE_LOG_WITHCTX_3(level, logctx_getter, what, reason, will)  \
    do {                                                                   \
        /* 确保参数的顺序正确 */                                  \
        __LOG_STATIC_ASSERT((__LOG_GET_FIELD what) == LOG_FIELD_WHAT);     \
        __LOG_STATIC_ASSERT((__LOG_GET_FIELD reason) == LOG_FIELD_REASON); \
        __LOG_STATIC_ASSERT((__LOG_GET_FIELD will) == LOG_FIELD_WILL);     \
        /* 确保以下字段不能太短 */                               \
        __LOG_STATIC_ASSERT(sizeof(__LOG_GET_FMT what) > 1);               \
        /* 调用真实的日志函数 */                                  \
        NORMALIZE_LOG_WITHCTX_API(                                         \
            level, logctx_getter,                                          \
            __LOG_GET_FMT what __LOG_GET_FMT reason __LOG_GET_FMT how_to   \
                __LOG_GET_ARGS what __LOG_GET_ARGS reason __LOG_GET_ARGS   \
                    will);                                                 \
    } while (0)

/**
 * @brief 2要素宏定义(支持日志上下文)
 */
#define NORMALIZE_LOG_WITHCTX_2(level, logctx_getter, what, reason)        \
    do {                                                                   \
        /* 确保参数的顺序正确 */                                  \
        __LOG_STATIC_ASSERT((__LOG_GET_FIELD what) == LOG_FIELD_WHAT);     \
        __LOG_STATIC_ASSERT((__LOG_GET_FIELD reason) == LOG_FIELD_REASON); \
        /* 确保以下字段不能太短 */                               \
        __LOG_STATIC_ASSERT(sizeof(__LOG_GET_FMT what) > 1);               \
        /* 调用真实的日志函数 */                                  \
        NORMALIZE_LOG_WITHCTX_API(                                         \
            level, logctx_getter,                                          \
            __LOG_GET_FMT what __LOG_GET_FMT reason __LOG_GET_ARGS what    \
                __LOG_GET_ARGS reason);                                    \
    } while (0)

/**
 * @brief 1要素宏定义(支持日志上下文)
 */
#define NORMALIZE_LOG_WITHCTX_1(level, logctx_getter, what)                \
    do {                                                                   \
        /* 确保参数的顺序正确 */                                  \
        __LOG_STATIC_ASSERT((__LOG_GET_FIELD what) == LOG_FIELD_WHAT);     \
        /* 确保以下字段不能太短 */                               \
        __LOG_STATIC_ASSERT(sizeof(__LOG_GET_FMT what) > 1);               \
        /* 调用真实的日志函数 */                                  \
        NORMALIZE_LOG_WITHCTX_API(level, logctx_getter,                    \
                                  __LOG_GET_FMT what __LOG_GET_ARGS what); \
    } while (0)

#endif  // CLION_CPPSTUDY_NORMALIZE_LOG_H
