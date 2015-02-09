#import <AppKit/NSView.h>
#import <AppKit/NSWindow.h>
#import <AppKit/NSApplication.h>
#import <AppKit/NSShadow.h>
#import <AppKit/NSColor.h>

#include <QMainWindow>

void fixNativeWindow( QMainWindow * win )
{
  NSView *view = (NSView *)win->winId();
  NSWindow *window = [view window];
  [window setStyleMask: NSResizableWindowMask | NSClosableWindowMask |  NSMiniaturizableWindowMask | NSBorderlessWindowMask ];
//  [window setHasShadow:YES];
//  NSShadow *dropShadow = [[NSShadow alloc] init];
//        [dropShadow setShadowColor:[NSColor blackColor]];
//        [dropShadow setShadowOffset:NSMakeSize(0, -3)];
//        [dropShadow setShadowBlurRadius:10.0];
//
//        [view setWantsLayer: YES];
//        [view setShadow: dropShadow];
//
//        [dropShadow release];
}

void minaturize( QMainWindow * win )
{
	NSView *view = (NSView *)win->winId();
	NSWindow *window = [view window];
	[window miniaturize:window];
}

