#ifndef ADSGLOBAL_H
#define ADSGLOBAL_H

#include <QObject>
#include <QFlags>
#include <QDebug>

#if defined(ADS_IMPORT)
    #define ADS_EXPORT_API
#elif defined(ADS_EXPORT)
    #define ADS_EXPORT_API Q_DECL_EXPORT
#else
    #define ADS_EXPORT_API Q_DECL_IMPORT
#endif

#ifdef ADS_DEBUG_PRINT
    #define ADS_PRINT(s) qDebug() << s
#else
    #define ADS_PRINT(s)
#endif

// ads 命名空间
#ifdef ADS_NAMESPACE_ENABLED
    #define ADS_NAMESPACE_BEGIN namespace ads {
    #define ADS_NAMESPACE_END }
    #define ADS_NS ::ads
#else
    #define ADS_NAMESPACE_BEGIN
    #define ADS_NAMESPACE_END
    #define ADS_NS
#endif

// 本机窗口边框的宽度（基于操作系统）
#define ADS_WINDOW_FRAME_BORDER_WIDTH 7

#endif // ADSGLOBAL_H
