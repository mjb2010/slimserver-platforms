/*
 * Copyright (c) 2002 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.2 (the
 * "License").  You may not use this file except in compliance with the
 * License.  Please obtain a copy of the License at
 * http://www.apple.com/publicsource and read it before using this file.
 * 
 * This Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
/*
    File:       mDNSPlatformFunctions.h

    Contains:   Prototype for functions implemented and callable by the platform.

    Written by: Stuart Cheshire

    Version:    mDNS Core, September 2002

    Copyright:  Copyright (c) 2002 by Apple Computer, Inc., All Rights Reserved.

    Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple Computer, Inc.
                ("Apple") in consideration of your agreement to the following terms, and your
                use, installation, modification or redistribution of this Apple software
                constitutes acceptance of these terms.  If you do not agree with these terms,
                please do not use, install, modify or redistribute this Apple software.

                In consideration of your agreement to abide by the following terms, and subject
                to these terms, Apple grants you a personal, non-exclusive license, under Apple's
                copyrights in this original Apple software (the "Apple Software"), to use,
                reproduce, modify and redistribute the Apple Software, with or without
                modifications, in source and/or binary forms; provided that if you redistribute
                the Apple Software in its entirety and without modifications, you must retain
                this notice and the following text and disclaimers in all such redistributions of
                the Apple Software.  Neither the name, trademarks, service marks or logos of
                Apple Computer, Inc. may be used to endorse or promote products derived from the
                Apple Software without specific prior written permission from Apple.  Except as
                expressly stated in this notice, no other rights or licenses, express or implied,
                are granted by Apple herein, including but not limited to any patent rights that
                may be infringed by your derivative works or by other works in which the Apple
                Software may be incorporated.

                The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
                WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
                WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
                PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN
                COMBINATION WITH YOUR PRODUCTS.

                IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
                CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
                GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
                ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR DISTRIBUTION
                OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT
                (INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN
                ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    Change History (most recent first):

$Log: mDNSPlatformFunctions.h,v $
Revision 1.1  2003/07/18 19:41:52  dean
Initial revision

Revision 1.1  2003/01/20 00:45:56  blackketter
Adding support for advertising the SLIMP3 via zeroconf/mDNS/Rendezvous.

Debugging via --d_mdns

Requires a binary executable installed to be fork/exec'ed off for each
instance advertised.  Need to add an option for disabling it.

It advertises the web interface, the HTTP automation interface (which are
the same now) and the CLI.  The latter two are _slimdevices_slimp3_http._tcp
and _slimdevices_slimp3_cli._tcp respectively.

This uses the Apple open source implementation of the mDNS
command-line application mDNSResponderPosix.

This source is included so folks can build the other posix implementations.
Windows, as always, will require more work.

Rumor has it that there will be a CPAN module for zeroconf, which would obsolete
this implementation if pulled in.  That's ok, we'll take it out in that case.

This introduces a new directory in slimp3/server which is bin.  bin contains
subdirectories for each supported platform and is where we'd put executables
we use (similar to the lib/CPAN arrangement.)

Also added the submitted patch for ogginfo support. This gives ogg files the
opportunity to be browsed by meta information.  This requires several ogg
executables to be installed in the default path, which is too hard for the
average user.  This should also be cleaned up.

The Ogg patch also included a fix to make ogg files work with spaces in the
filename.

Tweaked the comments for the fix that Rob Moser added when I broke hiding
. and .. on Win32 systems.

Revision 1.10  2002/09/21 20:44:49  zarzycki
Added APSL info

Revision 1.9  2002/09/19 04:20:43  cheshire
Remove high-ascii characters that confuse some systems

Revision 1.8  2002/09/16 23:12:14  cheshire
Minor code tidying

Revision 1.7  2002/09/16 18:41:42  cheshire
Merge in license terms from Quinn's copy, in preparation for Darwin release

*/

#ifndef __mDNSPlatformFunctions_h
#define __mDNSPlatformFunctions_h

// ***************************************************************************
// Support functions which must be provided by each set of specific PlatformSupport files

// mDNSPlatformInit() typically opens a communication endpoint, and starts listening for mDNS packets.
// When Setup is complete, the callback is called.
// mDNSPlatformSendUDP() sends one UDP packet
// When a packet is received, the PlatformSupport code calls mDNSCoreReceive()
// mDNSPlatformScheduleTask() indicates that a timer should be set,
// and mDNSCoreTask() should be called when the timer expires
// mDNSPlatformClose() tidies up on exit

#ifdef	__cplusplus
	extern "C" {
#endif

// ***************************************************************************
// DNS protocol message format

typedef struct
	{
	mDNSOpaque16 id;
	mDNSOpaque16 flags;
	mDNSu16 numQuestions;
	mDNSu16 numAnswers;
	mDNSu16 numAuthorities;
	mDNSu16 numAdditionals;
	} DNSMessageHeader;

// We can send and receive packets up to 9000 bytes (Ethernet Jumbo Frame size, if that ever becomes widely used)
// However, in the normal case we try to limit packets to 1500 bytes so that we don't get IP fragmentation on standard Ethernet
#define AbsoluteMaxDNSMessageData 8960
#define NormalMaxDNSMessageData 1460
typedef struct
	{
	DNSMessageHeader h;						// Note: Size 12 bytes
	mDNSu8 data[AbsoluteMaxDNSMessageData];	// 20 (IP) + 8 (UDP) + 12 (header) + 8960 (data) = 9000
	} DNSMessage;

// ***************************************************************************
// Functions

// Every platform support module must provide the following functions
extern mStatus  mDNSPlatformInit   (mDNS *const m);
extern void     mDNSPlatformClose  (mDNS *const m);
extern mStatus  mDNSPlatformSendUDP(const mDNS *const m, const DNSMessage *const msg, const mDNSu8 *const end,
	mDNSIPAddr src, mDNSIPPort srcport, mDNSIPAddr dst, mDNSIPPort dstport);

extern mDNSs32  mDNSPlatformOneSecond;
extern mDNSs32  mDNSPlatformTimeNow();
extern void     mDNSPlatformScheduleTask(const mDNS *const m, mDNSs32 NextTaskTime);

extern void     mDNSPlatformLock        (const mDNS *const m);
extern void     mDNSPlatformUnlock      (const mDNS *const m);

extern void     mDNSPlatformStrCopy(const void *src,       void *dst);
extern mDNSu32  mDNSPlatformStrLen (const void *src);
extern void     mDNSPlatformMemCopy(const void *src,       void *dst, mDNSu32 len);
extern mDNSBool mDNSPlatformMemSame(const void *src, const void *dst, mDNSu32 len);
extern void     mDNSPlatformMemZero(                       void *dst, mDNSu32 len);

// The core mDNS code provides these functions, for the platform support code to call at appropriate times
extern void     mDNSCoreInitComplete(mDNS *const m, mStatus result);
extern void     mDNSCoreReceive(mDNS *const m, DNSMessage *const msg, const mDNSu8 *const end,
								mDNSIPAddr srcaddr, mDNSIPPort srcport, mDNSIPAddr dstaddr, mDNSIPPort dstport, mDNSIPAddr InterfaceAddr);
extern void     mDNSCoreTask   (mDNS *const m);
extern void     mDNSCoreSleep  (mDNS *const m, mDNSBool wake);

#ifdef	__cplusplus
	}
#endif

#endif