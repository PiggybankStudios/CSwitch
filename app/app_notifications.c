/*
File:   app_notifications.c
Author: Taylor Robbins
Date:   03\14\2025
Description: 
	** Holds the API for showing "Notifications" to the user, basically a way to
	** communicate important events that happened in a fire-and-forget way, where the
	** text will be displayed at the corner of the window for some period of time
	** and then automatically disappear. If multiple notifications happen quickly
	** then the notifications will stack on-top of eachother vertically and animate
	** nicely as they make space for each other. The notifications also animate in from
	** offscreen when they first appear and fade out as they go away, hopefully making the whole
	** experience feel less annoying compared to simple non-animated error reporting systems
*/

AppIcon GetAppIconForNotificationLevel(DbgLevel level, Color32* colorOut)
{
	SetOptionalOutPntr(colorOut, TEXT_WHITE);
	switch (level)
	{
		case DbgLevel_Warning: SetOptionalOutPntr(colorOut, MonokaiYellow); return AppIcon_NotificationWarning;
		case DbgLevel_Error: SetOptionalOutPntr(colorOut, ERROR_RED); return AppIcon_NotificationError;
		default: SetOptionalOutPntr(colorOut, GetDbgLevelTextColor(level)); return AppIcon_NotificationMessage;
	}
}

void InitNotificationQueue(Arena* arena, NotificationQueue* queueOut)
{
	NotNull(arena);
	NotNull(queueOut);
	ClearPointer(queueOut);
	queueOut->arena = arena;
	InitVarArray(Notification, &queueOut->notifications, arena);
	queueOut->nextId = 1;
}

void FreeNotification(Notification* notification)
{
	NotNull(notification);
	if (notification->arena != nullptr)
	{
		FreeStr8(notification->arena, &notification->messageStr);
	}
	ClearPointer(notification);
}

Notification* AddNotificationToQueue(NotificationQueue* queue, DbgLevel level, Str8 message) //pre-declared in app_notifications_api.h
{
	NotNull(appIn);
	NotNull(queue);
	NotNull(queue->arena);
	NotNullStr(message);
	
	//TODO: Check for existing duplicates of this message. Move them to the bottom and increment their repetition count!
	
	if (queue->notifications.length >= MAX_NOTIFICATIONS)
	{
		Notification* oldestNotification = VarArrayGetLast(Notification, &queue->notifications);
		FreeNotification(oldestNotification);
		VarArrayRemoveLast(Notification, &queue->notifications);
	}
	
	Notification* newNotification = VarArrayInsert(Notification, &queue->notifications, 0);
	NotNull(newNotification);
	ClearPointer(newNotification);
	newNotification->arena = queue->arena;
	newNotification->id = queue->nextId;
	queue->nextId++;
	newNotification->messageStr = AllocStr8(queue->arena, message);
	newNotification->spawnTime = appIn->programTime;
	newNotification->duration = DEFAULT_NOTIFICATION_TIME;
	newNotification->level = level;
	
	return newNotification;
}

void UpdateNotificationQueue(NotificationQueue* queue)
{
	NotNull(appIn);
	NotNull(queue);
	NotNull(queue->arena);
	
	rec prevNotificationDrawRec = Rec_Zero;
	VarArrayLoop(&queue->notifications, nIndex)
	{
		VarArrayLoopGet(Notification, notification, &queue->notifications, nIndex);
		
		// If we're halfway up the screen, auto-dismiss the notification by shortening the duration
		if (notification->gotoOffsetY >= appIn->screenSize.Height * (r32)NOTIFICATION_AUTO_DISMISS_SCREEN_HEIGHT_PERCENT)
		{
			u64 currentTime = TimeSinceBy(appIn->programTime, notification->spawnTime);
			if (currentTime < notification->duration - NOTIFICATION_DISAPPEAR_ANIM_TIME)
			{
				notification->duration = currentTime + NOTIFICATION_DISAPPEAR_ANIM_TIME;
			}
		}
		
		if (TimeSinceBy(appIn->programTime, notification->spawnTime) >= notification->duration)
		{
			FreeNotification(notification);
			VarArrayRemoveAt(Notification, &queue->notifications, nIndex);
			nIndex--;
			continue;
		}
		
		Str8 notificationIdStr = ScratchPrintStr("Notification%llu", (u64)notification->id);
		ClayId notificationId = ToClayId(notificationIdStr);
		rec notificationDrawRec = GetClayElementDrawRec(notificationId);
		if (!appIn->screenSizeChanged && prevNotificationDrawRec.Width > 0 && prevNotificationDrawRec.Height > 0)
		{
			notification->gotoOffsetY = ((r32)appIn->screenSize.Height - UI_R32(NOTIFICATION_SCREEN_MARGIN_BOTTOM)) - (prevNotificationDrawRec.Y - UI_R32(NOTIFICATION_BETWEEN_MARGIN));
		}
		prevNotificationDrawRec = notificationDrawRec;
		
		r32 offsetDiff = (notification->gotoOffsetY - notification->currentOffsetY);
		if (AbsR32(offsetDiff) >= 1.0f) { notification->currentOffsetY += offsetDiff / NOTIFICATION_MOVE_LAG; }
		else { notification->currentOffsetY = notification->gotoOffsetY; }
	}
}

void RenderNotificationQueue(NotificationQueue* queue)
{
	NotNull(appIn);
	NotNull(queue);
	NotNull(queue->arena);
	
	VarArrayLoop(&queue->notifications, nIndex)
	{
		VarArrayLoopGet(Notification, notification, &queue->notifications, nIndex);
		Str8 notificationIdStr = ScratchPrintStr("Notification%llu", (u64)notification->id);
		ClayId notificationId = ToClayId(notificationIdStr);
		
		u64 spawnAnimTime = TimeSinceBy(appIn->programTime, notification->spawnTime);
		r32 appearAnimAmount = 1.0f;
		r32 disappearAnimAmount = 0.0f;
		if (spawnAnimTime > notification->duration - NOTIFICATION_DISAPPEAR_ANIM_TIME)
		{
			disappearAnimAmount = (r32)(spawnAnimTime - (notification->duration - NOTIFICATION_DISAPPEAR_ANIM_TIME)) / (r32)NOTIFICATION_DISAPPEAR_ANIM_TIME;
		}
		else if (spawnAnimTime < NOTIFICATION_APPEAR_ANIM_TIME)
		{
			appearAnimAmount = (r32)spawnAnimTime / (r32)NOTIFICATION_APPEAR_ANIM_TIME;
		}
		
		Color32 backgroundColor = ColorWithAlpha(BACKGROUND_GRAY, 1.0f - disappearAnimAmount);
		Color32 textColor = ColorWithAlpha(TEXT_WHITE, 1.0f - disappearAnimAmount);
		Color32 borderColor = ColorWithAlpha(TEXT_GRAY, 1.0f - disappearAnimAmount);
		Color32 iconColor = TEXT_WHITE;
		AppIcon appIcon = GetAppIconForNotificationLevel(notification->level, &iconColor);
		Texture* iconTexture = (appIcon != AppIcon_None) ? &app->icons[appIcon] : nullptr;
		rec notificationDrawRec = GetClayElementDrawRec(notificationId);
		bool isSizeKnown = (notificationDrawRec.Width > 0);
		v2 offset = NewV2(
			-UI_R32(NOTIFICATION_SCREEN_MARGIN_RIGHT),
			-UI_R32(NOTIFICATION_SCREEN_MARGIN_BOTTOM) - notification->currentOffsetY
		);
		if (appearAnimAmount < 1.0f && isSizeKnown)
		{
			offset.X += notificationDrawRec.Width * EaseExponentialIn(1.0f - appearAnimAmount);
		}
		
		CLAY({ .id = notificationId,
			.layout = {
				.sizing = { .width = CLAY_SIZING_FIT(0, appIn->screenSize.Width*0.75f), .height = CLAY_SIZING_FIT(0) },
				.padding = CLAY_PADDING_ALL(UI_U16(NOTIFICATION_PADDING)),
				.childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER },
			},
			.floating = {
				.zIndex = 102,
				.offset = offset,
				.attachTo = CLAY_ATTACH_TO_PARENT,
				.pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_CAPTURE,
				.attachPoints = {
					.parent = CLAY_ATTACH_POINT_RIGHT_BOTTOM,
					.element = (isSizeKnown ? CLAY_ATTACH_POINT_RIGHT_BOTTOM : CLAY_ATTACH_POINT_LEFT_BOTTOM),
				},
			},
			.backgroundColor = backgroundColor,
			.cornerRadius = CLAY_CORNER_RADIUS(UI_R32(8)),
			.border = { .width = CLAY_BORDER_OUTSIDE(UI_BORDER(2)), .color = borderColor },
		})
		{
			CLAY({ .layout={ .layoutDirection=CLAY_LEFT_TO_RIGHT, .childGap=UI_U16(5), .childAlignment={ .y=CLAY_ALIGN_Y_CENTER } } })
			{
				if (iconTexture != nullptr)
				{
					CLAY_ICON(iconTexture, FillV2(UI_R32(NOTIFICATION_ICON_SIZE)), ColorWithAlpha(iconColor, 1.0f - disappearAnimAmount));
				}
				
				CLAY_TEXT(
					notification->messageStr,
					CLAY_TEXT_CONFIG({
						.fontId = app->clayUiFontId,
						.fontSize = (u16)app->uiFontSize,
						.textColor = textColor,
					})
				);
			}
		}
	}
}

