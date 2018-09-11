# Black Framework

Black is a high-performance SDK for build modern apps for desktop and mobile(iOS, Android) using same C++ code. 


### Example

*myapp.hpp*

    #include "platform/Application.hpp"
    #include "ui/Page.hpp"
  
    class MyApp : public Application{
      void initialize(){
        auto *page = new Page();
        page->addView(new Button("Hello word"));
        getPageController()->push(page);
      }
    }
    
*app.cpp*
  
    #include "myapp.hpp"
    MyApp app;
   
### Features
- Complete UI engine
- Reactive
- Api for video, audio and midi manipulation 
- Opengl & Vulkan backend
