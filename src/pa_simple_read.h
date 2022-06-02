#ifndef PA_SIMPLE_READ_H
#define PA_SIMPLE_READ_H

#include<thread>
#include<pulse/pulseaudio.h>
#include<pulse/simple.h>
#include<napi.h>

class Pulse : public Napi::ObjectWrap<Pulse>
{
  public: 
    static Napi::Object Init(Napi::Env env,Napi::Object exports);
    Pulse(const Napi::CallbackInfo& info);
  private:
    Napi::Value record(const Napi::CallbackInfo& info);
    Napi::Value stop(const Napi::CallbackInfo& info);
    std::thread nativeThread;
    Napi::ThreadSafeFunction tsfn;
    bool stop_flag;
};


#endif
