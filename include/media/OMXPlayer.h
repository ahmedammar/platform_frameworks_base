/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_OMXPLAYER_H
#define ANDROID_OMXPLAYER_H

#include <utils/Errors.h>
#include <media/MediaPlayerInterface.h>
#include <media/Metadata.h>

namespace android {

class OMXPlayer : public MediaPlayerInterface
{
public:
                        OMXPlayer(int nMediaType = 0);
    virtual             ~OMXPlayer();

    virtual status_t    initCheck();
    virtual status_t    setDataSource(const char *url, const KeyedVector<String8, String8> *headers);
    virtual status_t    setDataSource(int fd, int64_t offset, int64_t length);
    virtual status_t    setVideoSurface(const sp<ISurface>& surface);
    virtual status_t    prepare();
    virtual status_t    prepareAsync();
    virtual status_t    start();
    virtual status_t    stop();
    virtual status_t    pause();
    virtual bool        isPlaying();
    virtual status_t    seekTo(int msec);
    virtual status_t    getCurrentPosition(int *msec);
    virtual status_t    getDuration(int *msec);
    virtual status_t    reset();
    virtual status_t    setLooping(int loop);
    virtual player_type playerType() { return OMX_PLAYER; }
    virtual status_t    suspend();
    virtual status_t    resume();
    virtual status_t    invoke(const Parcel& request, Parcel *reply);
    virtual status_t    getMetadata(const SortedVector<media::Metadata::Type>& ids, Parcel *records);

    virtual status_t    setAudioEffect(int iBandIndex, int iBandFreq, int iBandGain);
    virtual status_t    setAudioEqualizer(bool isEnable);
    virtual status_t    captureCurrentFrame(VideoFrame** pvframe);
    virtual status_t    setVideoCrop(int top,int left, int bottom, int right);
    virtual int         getTrackCount();
    virtual char*       getTrackName(int index);
    virtual int         getDefaultTrack();
    virtual status_t    selectTrack(int index);

    void                sendEvent(int msg, int ext1=0, int ext2=0) { MediaPlayerBase::sendEvent(msg, ext1, ext2); }

    status_t            ProcessEvent(int eventID, void* Eventpayload);
    status_t            ProcessAsyncCmd();
    status_t            PropertyCheck();

private:
    typedef enum {
        MSG_NONE,
        MSG_PREPAREASYNC,
        MSG_SEEKTO,
    }MSGTYPE;

    void                *player;
    status_t            mInit;
    sp<ISurface>        mSurface;
    int                 mSharedFd;
    bool                bLoop;
    char                *contentURI;
    MSGTYPE             msg;
    int                 msgData;
    void                *sem;
    void                *pPCmdThread;
    bool                bStopPCmdThread;
    void                *pPCheckThread;
    bool                bStopPCheckThread;
    bool                bTvOut;
    bool                bDualDisplay;
    bool                bFB0Display;
    bool                bSuspend;
    int                 sLeft;
    int                 sRight;
    int                 sTop;
    int                 sBottom;
    int                 sRot;
	int                 mMediaType;
    status_t            setVideoDispRect(int top,int left, int bottom, int right);
    status_t            CheckSurfaceRegion();
    status_t            CheckDualDisplaySetting();
    status_t            CheckTvOutSetting();
    status_t            CheckFB0DisplaySetting();
    status_t            DoSeekTo(int msec);
};

class OMXPlayerType
{
public:
                        OMXPlayerType();
    virtual             ~OMXPlayerType();
    int                 IsSupportedContent(char *url);
};

}; // namespace android

#endif // ANDROID_OMXPLAYER_H