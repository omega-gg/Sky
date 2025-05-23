//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkCore.

    - GNU Lesser General Public License Usage:
    This file may be used under the terms of the GNU Lesser General Public License version 3 as
    published by the Free Software Foundation and appearing in the LICENSE.md file included in the
    packaging of this file. Please review the following information to ensure the GNU Lesser
    General Public License requirements will be met: https://www.gnu.org/licenses/lgpl.html.

    - Private License Usage:
    Sky kit licensees holding valid private licenses may use this file in accordance with the
    private license agreement provided with the Software or, alternatively, in accordance with the
    terms contained in written agreement between you and Sky kit authors. For further information
    contact us at contact@omega.gg.
*/
//=================================================================================================

#ifndef SK_NO_CONTROLLERAPPLICATION

// iOS includes
#import <UIKit/UIKit.h>
#import <AudioToolbox/AudioServices.h>
#import <MediaPlayer/MediaPlayer.h>
#import <sys/utsname.h>

// Qt includes
#include <QGuiApplication>
#include <QWindow>

// Sk includes
#include <WControllerApplication>
#include <WControllerFile>

// Private includes
#include <private/WControllerApplication_p>

//-------------------------------------------------------------------------------------------------
// NOTE iOS: These functions are required to trigger the local network permission dialog.
//-------------------------------------------------------------------------------------------------

#include <ifaddrs.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>

/// Returns the addresses of the discard service (port 9) on every
/// broadcast-capable interface.
///
/// Each array entry contains either a `sockaddr_in` or `sockaddr_in6`.
static NSArray<NSData *> * addressesOfDiscardServiceOnBroadcastCapableInterfaces(void) {
    struct ifaddrs * addrList = NULL;
    int err = getifaddrs(&addrList);
    if (err != 0) {
        return @[];
    }
    NSMutableArray<NSData *> * result = [NSMutableArray array];
    for (struct ifaddrs * cursor = addrList; cursor != NULL; cursor = cursor->ifa_next) {
        if ( (cursor->ifa_flags & IFF_BROADCAST) &&
             (cursor->ifa_addr != NULL)
           ) {
            switch (cursor->ifa_addr->sa_family) {
            case AF_INET: {
                struct sockaddr_in sin = *(struct sockaddr_in *) cursor->ifa_addr;
                sin.sin_port = htons(9);
                NSData * addr = [NSData dataWithBytes:&sin length:sizeof(sin)];
                [result addObject:addr];
            } break;
            case AF_INET6: {
                struct sockaddr_in6 sin6 = *(struct sockaddr_in6 *) cursor->ifa_addr;
                sin6.sin6_port = htons(9);
                NSData * addr = [NSData dataWithBytes:&sin6 length:sizeof(sin6)];
                [result addObject:addr];
            } break;
            default: {
                // do nothing
            } break;
            }
        }
    }
    freeifaddrs(addrList);
    return result;
}
/// Does a best effort attempt to trigger the local network privacy alert.
///
/// It works by sending a UDP datagram to the discard service (port 9) of every
/// IP address associated with a broadcast-capable interface interface. This
/// should trigger the local network privacy alert, assuming the alert hasn’t
/// already been displayed for this app.
///
/// This code takes a ‘best effort’. It handles errors by ignoring them. As
/// such, there’s guarantee that it’ll actually trigger the alert.
///
/// - note: iOS devices don’t actually run the discard service. I’m using it
/// here because I need a port to send the UDP datagram to and port 9 is
/// always going to be safe (either the discard service is running, in which
/// case it will discard the datagram, or it’s not, in which case the TCP/IP
/// stack will discard it).
///
/// There should be a proper API for this (r. 69157424).
///
/// For more background on this, see [Triggering the Local Network Privacy Alert](https://developer.apple.com/forums/thread/663768).
extern void triggerLocalNetworkPrivacyAlertObjC(void) {
    int sock4 = socket(AF_INET, SOCK_DGRAM, 0);
    int sock6 = socket(AF_INET6, SOCK_DGRAM, 0);

    if ((sock4 >= 0) && (sock6 >= 0)) {
        char message = '!';
        NSArray<NSData *> * addresses = addressesOfDiscardServiceOnBroadcastCapableInterfaces();
        for (NSData * address in addresses) {
            const struct sockaddr * bytes = (const struct sockaddr *) address.bytes;
            int sock = bytes->sa_family == AF_INET ? sock4 : sock6;
            (void) sendto(sock, &message, sizeof(message), MSG_DONTWAIT, bytes, (socklen_t) address.length);
        }
    }

    // If we failed to open a socket, the descriptor will be -1 and it’s safe to
    // close that (it’s guaranteed to fail with `EBADF`).
    close(sock4);
    close(sock6);
}

//-------------------------------------------------------------------------------------------------
// UIViewController
//-------------------------------------------------------------------------------------------------

@implementation UIViewController (SafeAreaNotifier)

- (void) viewSafeAreaInsetsDidChange
{
    dispatch_async(dispatch_get_main_queue(),
    ^{
        UIWindow *window = self.view.window;

        if (window == nil) return;

        UIEdgeInsets insets = window.safeAreaInsets;

        QMargins margins(insets.left, insets.top, insets.right, insets.bottom);

        QMetaObject::invokeMethod(qApp, [margins]
        {
            sk->setSafeMargins(margins);
        }, Qt::QueuedConnection);
    });
}

@end

//-------------------------------------------------------------------------------------------------
// WControllerApplicationImage
//-------------------------------------------------------------------------------------------------

@interface WControllerApplicationImage : NSObject

- (void) imagePickerController: (UIImagePickerController *) picker
         didFinishPickingMediaWithInfo: (NSDictionary *) info;

@end

@implementation WControllerApplicationImage

- (void) imagePickerController: (UIImagePickerController *) picker
         didFinishPickingMediaWithInfo: (NSDictionary *) info
{
    [picker dismissViewControllerAnimated: true completion: NULL];

    UIImage * image = info[UIImagePickerControllerEditedImage];

    if (image == NULL) image = info[UIImagePickerControllerOriginalImage];

    if (image == NULL) return;

    NSData * data = UIImagePNGRepresentation(image);

    if (data == NULL) return;

    QString path = wControllerFile->pathTemp() + "/temp.png";

    if ([data writeToFile: path.toNSString() atomically: false])
    {
        emit sk->imageSelected(path);
    }
}

@end

//-------------------------------------------------------------------------------------------------
// WControllerApplicationShare
//-------------------------------------------------------------------------------------------------

@interface WControllerApplicationShare : UIViewController<UIDocumentInteractionControllerDelegate>

- (UIViewController *) documentInteractionControllerViewControllerForPreview:
                       (UIDocumentInteractionController *) controller;

- (void) documentInteractionControllerDidEndPreview:
         (UIDocumentInteractionController *) controller;

@end

@implementation WControllerApplicationShare

- (UIViewController *) documentInteractionControllerViewControllerForPreview:
                       (UIDocumentInteractionController *) controller
{
    return self;
}

- (void) documentInteractionControllerDidEndPreview: (UIDocumentInteractionController *) controller
{
    // FIMXE iOS: For some reason we need to call this manually to reactivate the window.
    QGuiApplication::allWindows().first()->requestActivate();

    emit sk->shareFinished(true);
}

@end

//-------------------------------------------------------------------------------------------------
// WControllerApplicationPlayback
//-------------------------------------------------------------------------------------------------

@interface WControllerApplicationPlayback : NSObject

- (void) registerEvents;

@end

@implementation WControllerApplicationPlayback

- (void) registerEvents
{
    MPRemoteCommandCenter * center = [MPRemoteCommandCenter sharedCommandCenter];

    [center.playCommand  addTarget:self action:@selector(onEvent:)];
    [center.pauseCommand addTarget:self action:@selector(onEvent:)];
    [center.stopCommand  addTarget:self action:@selector(onEvent:)];
}

- (MPRemoteCommandHandlerStatus) onEvent: (MPRemoteCommandEvent *) event
{
    MPRemoteCommandCenter * center = [MPRemoteCommandCenter sharedCommandCenter];

    if (event.command == center.playCommand)
    {
        emit sk->playbackUpdated(WControllerApplication::Play);

        return MPRemoteCommandHandlerStatusSuccess;
    }
    else if (event.command == center.pauseCommand)
    {
        emit sk->playbackUpdated(WControllerApplication::Pause);

        return MPRemoteCommandHandlerStatusSuccess;
    }
    else if (event.command == center.stopCommand)
    {
        emit sk->playbackUpdated(WControllerApplication::Stop);

        return MPRemoteCommandHandlerStatusSuccess;
    }
    return MPRemoteCommandHandlerStatusCommandFailed;
}

@end

//-------------------------------------------------------------------------------------------------
// WControllerApplicationPrivate
//-------------------------------------------------------------------------------------------------

void WControllerApplicationPrivate::setScreenSaverEnabled(bool enabled)
{
    [[UIApplication sharedApplication] setIdleTimerDisabled: !(enabled)];
}

//-------------------------------------------------------------------------------------------------
// WControllerApplication
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerApplication::deviceName()
{
    UIDevice * device = [UIDevice currentDevice];

    return QString::fromNSString(device.model);
}

/* Q_INVOKABLE static */ QString WControllerApplication::deviceVersion()
{
    NSString * version = [[UIDevice currentDevice] systemVersion];

    return QString::fromNSString(version);
}

/* Q_INVOKABLE static */ int WControllerApplication::orientation()
{
    UIDeviceOrientation orientation = [[UIDevice currentDevice] orientation];

    if      (orientation == UIDeviceOrientationLandscapeLeft)      return 90;
    else if (orientation == UIDeviceOrientationPortraitUpsideDown) return 180;
    else if (orientation == UIDeviceOrientationLandscapeRight)     return 270;
    else                                                           return 0;
}

/* Q_INVOKABLE static */
int WControllerApplication::orientationCamera(int orientation, const QString & id)
{
    if (@available(iOS 17.0, *))
    {
        Q_UNUSED(orientation); Q_UNUSED(id);

        return 0;
    }
#ifdef QT_6
    // NOTE iOS: When the id ends with :1 it's usually the front facing camera.
    else if (id.endsWith(":1"))
    {
        if (orientation == 90)  return 180;
        if (orientation == 180) return 90;
        if (orientation == 270) return 0;
        else                    return 270;
    }
    else if (orientation == 90)  return 0;
    else if (orientation == 180) return 270;
    else if (orientation == 270) return 180;
    else                         return 90;
#else
    else return 0;
#endif
}

/* Q_INVOKABLE static */ void WControllerApplication::forceLandscape(bool enabled)
{
    if (@available(iOS 16.0, *))
    {
        UIWindowScene * window = nil;

        for (UIScene * scene in [UIApplication sharedApplication].connectedScenes)
        {
            if ([scene isKindOfClass:[UIWindowScene class]]
                /*&&
                scene.activationState == UISceneActivationStateForegroundActive*/)
            {
                window = (UIWindowScene *) scene;

                break;
            }
        }

        if (window == nil) return;

        UIInterfaceOrientationMask orientation;

        if (enabled) orientation = UIInterfaceOrientationMaskLandscape;
        else         orientation = UIInterfaceOrientationMaskPortrait;

        UIWindowSceneGeometryPreferencesIOS * preferences
            = [[UIWindowSceneGeometryPreferencesIOS alloc]
                initWithInterfaceOrientations: orientation];

        [window requestGeometryUpdateWithPreferences:preferences errorHandler:nil];
    }
}

/* Q_INVOKABLE static */ void WControllerApplication::showPlayback(const QString & title,
                                                                   const QString & author)
{
    static WControllerApplicationPlayback * playback = NULL;

    if (playback == NULL)
    {
        playback = [[WControllerApplicationPlayback alloc] init];

        [playback registerEvents];
    }

    NSMutableDictionary * info = [NSMutableDictionary dictionary];

    info[MPMediaItemPropertyTitle] = title.toNSString();

    if (author.isEmpty() == false)
    {
        info[MPMediaItemPropertyArtist] = author.toNSString();
    }

    [MPNowPlayingInfoCenter defaultCenter].nowPlayingInfo = info;
}

/* Q_INVOKABLE static */ void WControllerApplication::hidePlayback()
{
    [MPNowPlayingInfoCenter defaultCenter].nowPlayingInfo = nil;
}

/* Q_INVOKABLE static */ void WControllerApplication::vibrate(int)
{
    AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
}

/* Q_INVOKABLE static */ void WControllerApplication::openGallery()
{
    if ([UIImagePickerController
         isSourceTypeAvailable: (UIImagePickerControllerSourceType) 0] == false) return;

    UIApplication * application = [UIApplication sharedApplication];

    if (application.windows.count == 0) return;

    UIWindow * root = application.windows[0];

    UIImagePickerController * picker = [[UIImagePickerController alloc] init];

    picker.delegate = [WControllerApplicationImage alloc];

    [root.rootViewController presentViewController: picker animated: true completion: NULL];
}

/* Q_INVOKABLE static */ void WControllerApplication::shareText(const QString & text)
{
    UIApplication * application = [UIApplication sharedApplication];

    if (application.windows.count == 0) return;

    NSMutableArray * items = [NSMutableArray new];

    QUrl url(text);

    // NOTE iOS: We check against the scheme because isValid is too tolerant.
    if (url.scheme().isEmpty())
    {
         [items addObject:text.toNSString()];
    }
    else [items addObject:url.toNSURL()];

    UIViewController * controller = application.windows[0].rootViewController;

    UIActivityViewController * activity = [[UIActivityViewController alloc]
                                           initWithActivityItems: items
                                           applicationActivities: NULL];

    [controller presentViewController: activity animated: true completion: NULL];
}

/* Q_INVOKABLE static */ void WControllerApplication::shareFile(const QString & fileName)
{
    UIApplication * application = [UIApplication sharedApplication];

    if (application.windows.count == 0) return;

    static WControllerApplicationShare * share = NULL;

    // NOTE: This condition might be useless.
    if (share)
    {
        [share removeFromParentViewController];
        [share release];
    }

    share = [[WControllerApplicationShare alloc] init];

    UIViewController * controller = application.windows[0].rootViewController;

    [controller addChildViewController: share];

    NSURL * url = [NSURL fileURLWithPath: fileName.toNSString()];

    UIDocumentInteractionController * document =
        [UIDocumentInteractionController interactionControllerWithURL: url];

    document.delegate = share;

    [document presentPreviewAnimated: true];
}

/* Q_INVOKABLE static */ void WControllerApplication::triggerLocal()
{
    triggerLocalNetworkPrivacyAlertObjC();
}

#endif // SK_NO_CONTROLLERAPPLICATION
