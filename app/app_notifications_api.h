/*
File:   app_notifications_api.h
Author: Taylor Robbins
Date:   03\15\2025
*/

#ifndef _APP_NOTIFICATIONS_API_H
#define _APP_NOTIFICATIONS_API_H

Notification* AddNotificationToQueue(NotificationQueue* queue, DbgLevel level, Str8 message);

#define NotifyAt(level, message)                AddNotificationToQueue(&app->notifications, (level),          StrLit(message))
#define Notify_D(message)                       AddNotificationToQueue(&app->notifications, DbgLevel_Debug,   StrLit(message))
#define Notify_R(message)                       AddNotificationToQueue(&app->notifications, DbgLevel_Regular, StrLit(message))
#define Notify_I(message)                       AddNotificationToQueue(&app->notifications, DbgLevel_Info,    StrLit(message))
#define Notify_N(message)                       AddNotificationToQueue(&app->notifications, DbgLevel_Notify,  StrLit(message))
#define Notify_O(message)                       AddNotificationToQueue(&app->notifications, DbgLevel_Other,   StrLit(message))
#define Notify_W(message)                       AddNotificationToQueue(&app->notifications, DbgLevel_Warning, StrLit(message))
#define Notify_E(message)                       AddNotificationToQueue(&app->notifications, DbgLevel_Error,   StrLit(message))

#define NotifyPrintAt(level, formatString, ...) AddNotificationToQueue(&app->notifications, (level),          ScratchPrintStr(formatString, ##__VA_ARGS__))
#define NotifyPrint_D(formatString, ...)        AddNotificationToQueue(&app->notifications, DbgLevel_Debug,   ScratchPrintStr(formatString, ##__VA_ARGS__))
#define NotifyPrint_R(formatString, ...)        AddNotificationToQueue(&app->notifications, DbgLevel_Regular, ScratchPrintStr(formatString, ##__VA_ARGS__))
#define NotifyPrint_I(formatString, ...)        AddNotificationToQueue(&app->notifications, DbgLevel_Info,    ScratchPrintStr(formatString, ##__VA_ARGS__))
#define NotifyPrint_N(formatString, ...)        AddNotificationToQueue(&app->notifications, DbgLevel_Notify,  ScratchPrintStr(formatString, ##__VA_ARGS__))
#define NotifyPrint_O(formatString, ...)        AddNotificationToQueue(&app->notifications, DbgLevel_Other,   ScratchPrintStr(formatString, ##__VA_ARGS__))
#define NotifyPrint_W(formatString, ...)        AddNotificationToQueue(&app->notifications, DbgLevel_Warning, ScratchPrintStr(formatString, ##__VA_ARGS__))
#define NotifyPrint_E(formatString, ...)        AddNotificationToQueue(&app->notifications, DbgLevel_Error,   ScratchPrintStr(formatString, ##__VA_ARGS__))

#endif //  _APP_NOTIFICATIONS_API_H
