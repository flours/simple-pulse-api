#include "pa_simple_read.h"
#include <iostream>
#define DATA_SIZE 1024

Napi::Object Pulse::Init(Napi::Env env,Napi::Object exports){
  Napi::Function func =
      DefineClass(env,
                  "pulse",
                  {InstanceMethod("record", &Pulse::record),
                  InstanceMethod("stop", &Pulse::stop)});

  Napi::FunctionReference* constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(func);
  env.SetInstanceData(constructor);

  exports.Set("pulse", func);
  return exports;
}

Pulse::Pulse(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Pulse>(info){
}


// callback function,rate int,streamname string;
Napi::Value Pulse::record(const Napi::CallbackInfo& info){
  Napi::Env env = info.Env();
  if ( info.Length() < 3 )
  {
    throw Napi::TypeError::New( env, "Expected three arguments" );
  }
  else if ( !info[0].IsFunction() )
  {
    throw Napi::TypeError::New( env, "Expected first arg to be function" );
  }
  else if ( !info[1].IsNumber() )
  {
    throw Napi::TypeError::New( env, "Expected second arg to be number" );
  }
  else if ( !info[2].IsString() )
  {
    throw Napi::TypeError::New( env, "Expected third arg to be string" );
  }
  int sampling_rate= info[1].As<Napi::Number>().Int32Value();
  std::string source_name= info[2].As<Napi::String>().ToString();

  // Create a ThreadSafeFunction
  tsfn = Napi::ThreadSafeFunction::New(
      env,
      info[0].As<Napi::Function>(),  // JavaScript function called asynchronously
      "Resource Name",         // Name
      0,                       // Unlimited queue
      1,                       // Only one thread will use this initially
      [this]( Napi::Env ) {        // Finalizer used to clean threads up
        nativeThread.join();
      } );

  stop_flag=false;
  nativeThread = std::thread( [this,sampling_rate,source_name] {
    std::cout<<__LINE__;
    int pa_errno,pa_result;
    pa_sample_spec ss;
    ss.rate = sampling_rate;
    ss.format = PA_SAMPLE_S16LE;
    ss.channels=1;
    std::cout<<source_name<<",source_name";
    pa_simple *pa = pa_simple_new(NULL,"pulse_simple", PA_STREAM_RECORD, source_name.c_str(),"record_stream", &ss, NULL, NULL, &pa_errno);
    if (pa == NULL) {
      std::cout<<"pa new Error";
    }
    auto callback = []( Napi::Env env, Napi::Function jsCallback, char* value ) {
      // Transform native data into JS data, passing it to the provided
      // `jsCallback` -- the TSFN's JavaScript function.
      jsCallback.Call( {Napi::Buffer<char>::Copy( env, value,(size_t)DATA_SIZE )} );

      // We're finished with the data.
      //delete value;
    };

    char data[DATA_SIZE];
    while(true)
    {
      // Create new data
      pa_result = pa_simple_read(pa,data,DATA_SIZE,&pa_errno);
      if(pa_result<0){
        break;
      }
      FILE *fp=fopen("record.pcm","ab+");
      fwrite(data,sizeof(char),DATA_SIZE,fp);
      fclose(fp);
      if(stop_flag)break;
      // Perform a blocking call
      napi_status status = tsfn.BlockingCall(data, callback );
      if ( status != napi_ok )
      {
        // Handle error
        break;
      }
    }
    pa_simple_free(pa);
    // Release the thread-safe function
    tsfn.Release();
  });
  return Napi::Boolean::New(env,true);
}


Napi::Value Pulse::stop(const Napi::CallbackInfo& info){
  Napi::Env env = info.Env();
  stop_flag=true;
  return Napi::Boolean::New(env,true);
}

Napi::Object InitAll(Napi::Env env,Napi::Object exports) {
  return Pulse::Init(env,exports);
}

NODE_API_MODULE(addon,InitAll);
